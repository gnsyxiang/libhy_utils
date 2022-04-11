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

/* According to POSIX.1-2001, POSIX.1-2008 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_time.h"

#include "hy_list.h"

#include "hy_timer_multi_wheel.h"

#define _LIST_BASE_BIT      (8)
#define _LIST_BASE_SIZE     (0x1U << _LIST_BASE_BIT)
#define _LIST_BASE_MASK     (_LIST_BASE_SIZE - 1)

#define _LIST_BIT           (6)
#define _LIST_SIZE          (0x1U << _LIST_BIT)
#define _LIST_MASK          (_LIST_SIZE - 1)

#define _SELECT_TIME_MS     (1)

#define _INDEX(_cur_ms, _n) \
    (((_cur_ms) >> (_LIST_BASE_BIT + (_n) * _LIST_BIT)) & _LIST_MASK)

typedef struct {
    HyTimerMultiWheelConfig_s   timer_c;

    struct hy_list_head         entry;
} _timer_s;

typedef struct {
    struct hy_list_head         list_base[_LIST_BASE_SIZE];
} _timer_list_base_s;

typedef struct {
    struct hy_list_head         list[_LIST_SIZE];
} _timer_list_s;

typedef struct {
    _timer_list_base_s          list_base;
    _timer_list_s               list[4];

    hy_u64_t                    cur_ms;

    hy_s32_t                    exit_flag;
    void                        *thread_h;
} _timer_context_s;

static hy_s32_t is_init = 0;
static _timer_context_s *context = NULL;

static hy_s32_t _timer_add(_timer_s *timer, hy_u64_t expires)
{
    struct hy_list_head *list;
    hy_u32_t index = 0;
    hy_u32_t expires_time = timer->timer_c.expires;
    hy_u64_t idx = expires - context->cur_ms;

    if ((hy_s64_t)idx < 0) {
        // 定时器已经超时，放在当前位置直接忽略
        index = context->cur_ms & _LIST_BASE_MASK;
        list = context->list_base.list_base + index;
    } else if (idx < (0x1UL << (_LIST_BASE_BIT + 0 * _LIST_BIT))) {
        index = expires_time & _LIST_BASE_MASK;
        list = context->list_base.list_base + index;
    } else if (idx < (0x1UL << (_LIST_BASE_BIT + 1 * _LIST_BIT))) {
        index = (expires_time >> (_LIST_BASE_BIT + 0 * _LIST_BIT)) & _LIST_MASK;
        list = context->list[0].list + index;
    } else if (idx < (0x1UL << (_LIST_BASE_BIT + 2 * _LIST_BIT))) {
        index = (expires_time >> (_LIST_BASE_BIT + 1 * _LIST_BIT)) & _LIST_MASK;
        list = context->list[1].list + index;
    } else if (idx < (0x1UL << (_LIST_BASE_BIT + 3 * _LIST_BIT))) {
        index = (expires_time >> (_LIST_BASE_BIT + 2 * _LIST_BIT)) & _LIST_MASK;
        list = context->list[2].list + index;
    } else {
        if (idx > 0xffffffffUL) {
            idx = 0xffffffffUL;
            expires_time = idx + context->cur_ms;
        }

        index = (expires_time >> (_LIST_BASE_BIT + 3 * _LIST_BIT)) & _LIST_MASK;
        list = context->list[3].list + index;
    }

    /* @fixme: <22-04-11, uos> 加锁处理 */
    hy_list_add_tail(&timer->entry, list);

    return 0;
}

void *HyTimerMultiWheelAdd(HyTimerMultiWheelConfig_s *timer_c)
{
    HY_ASSERT(timer_c);
    _timer_s *timer = NULL;
    hy_u64_t expires = 0;

    do {
        timer = HY_MEM_MALLOC_BREAK(_timer_s *, sizeof(*timer));
        HY_MEMCPY(&timer->timer_c, timer_c, sizeof(timer->timer_c));

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

    if (timer) {
        HY_MEM_FREE_PP(&timer);
    }

    return NULL;
}

static int _tw_cascade(_timer_list_s *list, int index)
{
    struct hy_list_head *pos, *tmp;
    struct hy_list_head list_tmp;
    _timer_s *timer;

    /* re calc timewheel all task */
    hy_list_replace_init(list->list + index, &list_tmp);

    hy_list_for_each_safe(pos, tmp, &list_tmp) {
        timer = hy_list_entry(pos, _timer_s, entry);
        _timer_add(timer, timer->timer_c.expires);
    }

    return index;
}

static hy_s32_t _timer_thread_cb(void *args)
{
    hy_u64_t ms;
    _timer_s *pos, *n;
    HyTimerMultiWheelConfig_s *timer_c = NULL;

    ms = HyTimeGetUTCMs();
    do {
        while (context->cur_ms <= ms) {
            /* some timer expire */
            struct hy_list_head work_list;
            struct hy_list_head *head = &work_list;
            hy_s32_t index = context->cur_ms & _LIST_BASE_MASK;

            if (!index
                    && (!_tw_cascade(&context->list[0], _INDEX(context->cur_ms, 0)))
                    && (!_tw_cascade(&context->list[1], _INDEX(context->cur_ms, 1)))
                    && (!_tw_cascade(&context->list[2], _INDEX(context->cur_ms, 2)))) {
                _tw_cascade(&context->list[3], _INDEX(context->cur_ms, 3));
            }

            context->cur_ms += 1;
            hy_list_replace_init(context->list_base.list_base + index, &work_list);

            while (!hy_list_empty(head)) {
                hy_list_for_each_entry_safe(pos, n, head, entry) {
                    hy_list_del(&pos->entry);

                    timer_c = &pos->timer_c;
                    if (timer_c->timer_cb) {
                        timer_c->timer_cb(timer_c->args);
                    }
                }
            }
        }
    } while (!context->exit_flag && (ms = HyTimeGetUTCMs()));

    return -1;
}

static void _init_list(struct hy_list_head *list, hy_u32_t cnt)
{
    for (hy_u32_t i = 0; i < cnt; ++i) {
        HY_INIT_LIST_HEAD(list + i);
    }
}

void HyTimerMultiWheelDestroy(void)
{
    context->exit_flag = 1;
    HyThreadDestroy(&context->thread_h);

    LOGI("timer multi wheel destroy, context: %p \n", context);
    HY_MEM_FREE_PP(&context);
}

void HyTimerMultiWheelCreate(void)
{
    if (is_init) {
        LOGI("There is no need to initialize the timer repeatedly \n");
        return;
    }

    do {
        context = HY_MEM_MALLOC_BREAK(_timer_context_s *, sizeof(*context));

        context->cur_ms = HyTimeGetUTCMs();

        _init_list(context->list_base.list_base, _LIST_BASE_SIZE);
        for (int i = 0; i < 4; ++i) {
            _init_list(context->list[i].list, _LIST_SIZE);
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

