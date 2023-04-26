/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_timer_multi_wheel.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    11/04 2022 13:59
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        11/04 2022      create the file
 * 
 *     last modified: 11/04 2022 13:59
 */
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_thread.h"
#include "hy_time.h"
#include "hy_list.h"

#include "hy_timer_multi_wheel.h"

// 2^32 = 2^8 + 4 * 2^6
#define _MULTI_WHEEL_CNT        (4)

#define _LIST_BASE_BIT          (8)
#define _LIST_BASE_SIZE         (0x1UL << _LIST_BASE_BIT)
#define _LIST_BASE_MASK         (_LIST_BASE_SIZE - 1)

#define _LIST_BIT               (6)
#define _LIST_SIZE              (0x1UL << _LIST_BIT)
#define _LIST_MASK              (_LIST_SIZE - 1)

#define _SHIFT_BIT(_n)          (_LIST_BASE_BIT + (_n) * _LIST_BIT)
#define _SHIFT_LEFT(_n)         (0x1UL << _SHIFT_BIT(_n))
#define _INDEX(_cur_ms, _n)     (((_cur_ms) >> _SHIFT_BIT(_n)) & _LIST_MASK)

#define _TIMER_NS_TO_S          (1000000000)
#define _TIMER_MS_TO_NS         (1000000)

typedef struct {
    HyTimerMultiWheelConfig_s   timer_c;

    struct hy_list_head         entry;
} _timer_s;

typedef struct {
    struct hy_list_head         list_base[_LIST_BASE_SIZE];
    struct hy_list_head         list[_LIST_SIZE][_MULTI_WHEEL_CNT];

    hy_u32_t                    tick_ms;
    hy_u32_t                    cur_ms;
    hy_s32_t                    tfd;
    hy_s32_t                    eplfd;
    struct itimerspec           its;

    hy_s32_t                    exit_flag;
    HyThread_s                  *thread_h;
} _timer_context_s;

static hy_s32_t is_init = 0;
static _timer_context_s *context = NULL;

static hy_s32_t _timer_add(_timer_s *timer, hy_u32_t expires)
{
    struct hy_list_head *list = NULL;
    hy_u32_t index = 0;
    hy_u32_t expires_time = timer->timer_c.expires;
    hy_u32_t idx = expires - context->cur_ms;

    if ((hy_s32_t)idx < 0) {
        // 定时器已经超时，放在当前位置直接忽略
        index = context->cur_ms & _LIST_BASE_MASK;
        list = context->list_base + index;
    } else if (idx < _SHIFT_LEFT(0)) {
        index = expires_time & _LIST_BASE_MASK;
        list = context->list_base + index;
    } else if (idx < _SHIFT_LEFT(1)) {
        index = _INDEX(expires_time, 0);
        list = context->list[0] + index;
    } else if (idx < _SHIFT_LEFT(2)) {
        index = _INDEX(expires_time, 1);
        list = context->list[1] + index;
    } else if (idx < _SHIFT_LEFT(3)) {
        index = _INDEX(expires_time, 2);
        list = context->list[2] + index;
    } else {
        if (idx > 0xffffffffU) {
            expires_time = 0xffffffffU + context->cur_ms;
        }

        index = _INDEX(expires_time, 3);
        list = context->list[3] + index;
    }

    /* @fixme: <22-04-11, uos> 加锁处理 */
    hy_list_add_tail(&timer->entry, list);

    return 0;
}

static void _timer_destroy(_timer_s *timer)
{
    if (timer) {
        HY_MEM_FREE_PP(&timer);
    }
}

static void *_timer_create(HyTimerMultiWheelConfig_s *timer_c)
{
    _timer_s *timer = NULL;
    hy_u32_t expires = 0;

    do {
        timer = HY_MEM_MALLOC_BREAK(_timer_s *, sizeof(*timer));
        HY_MEMCPY(&timer->timer_c, timer_c, sizeof(timer->timer_c));

        timer->timer_c.expires /= context->tick_ms;
        timer->timer_c.expires += context->cur_ms;
        if (timer->timer_c.expires == expires) {
            // 此时context->cur_ms等于0，线程调度引起的
            LOGE("add timer error \n");
            break;
        }

        if (!timer->timer_c.timer_cb) {
           LOGW("callback function not specified \n");
           break;
        }

        if (0 != _timer_add(timer, timer->timer_c.expires)) {
            LOGE("timer add failed \n");
            break;
        }

        return timer;
    } while (0);

    _timer_destroy(timer);
    return NULL;
}

void *HyTimerMultiWheelAdd(HyTimerMultiWheelConfig_s *timer_c)
{
    HY_ASSERT(timer_c);

    return _timer_create(timer_c);
}

static int _tw_cascade(struct hy_list_head *list, int index)
{
    struct hy_list_head list_tmp;
    _timer_s *pos, *n;

    // 上一级移除链表
    hy_list_replace_init(list + index, &list_tmp);

    // 重新加入下一级链表
    hy_list_for_each_entry_safe(pos, n, &list_tmp, entry) {
        _timer_add(pos, pos->timer_c.expires);
    }

    return index;
}

static void _handle_timer(_timer_context_s *context)
{
    hy_s32_t index = 0;
    _timer_s *pos, *n;
    struct hy_list_head work_list;
    struct hy_list_head *head = &work_list;
    HyTimerMultiWheelConfig_s *timer_c = NULL;

    HY_MEMSET(&work_list, sizeof(work_list));
    index = context->cur_ms & _LIST_BASE_MASK;

    for (hy_s32_t i = 0; i < _MULTI_WHEEL_CNT && !index; ++i) {
        if (_tw_cascade(context->list[i], _INDEX(context->cur_ms, i))) {
            break;
        }
    }

    context->cur_ms += 1;
    hy_list_replace_init(context->list_base + index, &work_list);

    while (!hy_list_empty(head)) {
        hy_list_for_each_entry_safe(pos, n, head, entry) {
            hy_list_del(&pos->entry);

            timer_c = &pos->timer_c;
            if (timer_c->timer_cb) {
                timer_c->timer_cb(timer_c->args);
            }

            _timer_destroy(pos);
        }
    }
}

static hy_s32_t _timer_thread_cb(void *args)
{
#define MX_EVNTS 10
    struct epoll_event evnts[MX_EVNTS];
    hy_s32_t ret = -1;
    struct itimerspec *its = &context->its;
    struct epoll_event ev;

    while (!context->exit_flag) {
        ret = epoll_wait(context->eplfd, evnts, MX_EVNTS, -1);
        if (-1 == ret) {
            LOGES("epoll_wait failed \n");
            break;
        }

        ret = epoll_ctl(context->eplfd, EPOLL_CTL_DEL, context->tfd, NULL);
        if (-1 == ret) {
            LOGES("epoll_ctl failed \n");
            break;
        }

        _handle_timer(context);

        its->it_value.tv_sec    = its->it_value.tv_sec  + context->its.it_interval.tv_sec;
        its->it_value.tv_nsec   = its->it_value.tv_nsec + context->its.it_interval.tv_nsec;
        if (its->it_value.tv_nsec >= _TIMER_NS_TO_S) {
            its->it_value.tv_sec++;
            its->it_value.tv_nsec -= _TIMER_NS_TO_S;
        }
        ret = timerfd_settime(context->tfd, TFD_TIMER_ABSTIME, its, NULL);
        if (-1 == ret) {
            LOGES("timerfd_settime failed \n");
            break;
        }

        ev.events   = EPOLLIN | EPOLLET;
        ev.data.ptr = context;
        ret = epoll_ctl(context->eplfd, EPOLL_CTL_ADD, context->tfd, &ev);
        if (-1 == ret) {
            LOGES("epoll_ctl failed \n");
            break;
        }
    }

    return -1;
}

static void _timerfd_destroy(hy_s32_t tfd)
{
    if (tfd > 0) {
        close(tfd);
    }
}

static hy_s32_t _timerfd_create(_timer_context_s *context, hy_u32_t expires_ms)
{
    struct timespec nw;

    do {
        context->tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
        if (-1 == context->tfd) {
            LOGES("timerfd_create failed \n");
            break;
        }

        if (-1 == clock_gettime(CLOCK_MONOTONIC, &nw)) {
            LOGES("clock_gettime failed \n");
            break;
        }

        context->its.it_value.tv_sec     = nw.tv_sec + expires_ms / 1000;
        context->its.it_value.tv_nsec    = nw.tv_nsec + (expires_ms * _TIMER_MS_TO_NS);
        if (context->its.it_value.tv_nsec >= _TIMER_NS_TO_S) {
            context->its.it_value.tv_sec++;
            context->its.it_value.tv_nsec -= _TIMER_NS_TO_S;
        }
        context->its.it_interval.tv_sec  = expires_ms / 1000;
        context->its.it_interval.tv_nsec = expires_ms * _TIMER_MS_TO_NS;

        if (-1 == timerfd_settime(context->tfd, TFD_TIMER_ABSTIME,
                    &context->its, NULL)) {
            LOGES("timerfd_settime failed \n");
            break;
        }

        LOGI("tfd: %d \n", context->tfd);
        return 0;
    } while (0);

    _timerfd_destroy(context->tfd);
    return -1;
}

static void _epoll_destroy(hy_s32_t epfd)
{
    if (epfd) {
        close(epfd);
    }
}

static hy_s32_t _epoll_create(_timer_context_s *context)
{
    struct epoll_event ev;

    do {
        context->eplfd = epoll_create1(0);
        if(-1 == context->eplfd){
            LOGES("epoll_create1 failed \n");
            break;
        }

        ev.events   = EPOLLIN | EPOLLET;
        ev.data.ptr = context;
        if(-1 == epoll_ctl(context->eplfd, EPOLL_CTL_ADD, context->tfd, &ev)) {
            LOGES("epoll_ctl failed \n");
            break;
        }

        return 0;
    } while (0);

    _epoll_destroy(context->eplfd);
    return -1;
}

void HyTimerMultiWheelDestroy(void)
{
    context->exit_flag = 1;
    HyThreadDestroy(&context->thread_h);

    _timerfd_destroy(context->tfd);
    _epoll_destroy(context->eplfd);

    LOGI("timer multi wheel destroy, context: %p \n", context);
    HY_MEM_FREE_PP(&context);
}

void HyTimerMultiWheelCreate(hy_u32_t tick_ms)
{
    if (is_init) {
        LOGI("There is no need to initialize the timer repeatedly \n");
        return;
    }

    do {
        context = HY_MEM_MALLOC_BREAK(_timer_context_s *, sizeof(*context));

        context->cur_ms = 0;
        context->tick_ms = tick_ms;

        for (hy_u32_t i = 0; i < _LIST_BASE_SIZE; ++i) {
            HY_INIT_LIST_HEAD(context->list_base + i);
        }

        for (int i = 0; i < _MULTI_WHEEL_CNT; ++i) {
            for (hy_u32_t j = 0; j < _LIST_SIZE; ++j) {
                HY_INIT_LIST_HEAD(context->list[i] + j);
            }
        }

        if (-1 == _timerfd_create(context, tick_ms)) {
            LOGE("create timer fd failed \n");
            break;
        }

        if (-1 == _epoll_create(context)) {
            LOGE("create epoll failed \n");
            break;
        }

        context->thread_h = HyThreadCreate_m("HY_MW_timer",
                _timer_thread_cb, context);
        if (!context->thread_h) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        LOGI("timer multi wheel create, context: %p \n", context);
        return;
    } while (0);

    LOGE("timer multi wheel create failed \n");
    HyTimerMultiWheelDestroy();
}

