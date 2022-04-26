/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_timer.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 13:37
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 13:37
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_thread_mutex.h"
#include "hy_hal/hy_list.h"

#include "hy_timer.h"

typedef struct {
    HyTimerConfig_s         timer_c;

    hy_u32_t                rotation;           ///< 旋转的圈数，类比分针走一圈

    struct hy_list_head     list;
} _timer_t;

typedef struct {
    hy_u32_t                slot_interval_ms;
    hy_u32_t                slot_num;

    hy_u32_t                cur_slot;
    void                    *mutex_h;

    hy_s32_t                exit_flag;
    void                    *thread_h;

    struct hy_list_head     *list_head;
} _timer_context_t;

static hy_s32_t _is_init = 0;
static _timer_context_t context;

void *HyTimerAdd(HyTimerConfig_s *timer_c)
{
    LOGT("timer_c: %p \n", timer_c);
    HY_ASSERT_RET_VAL(!timer_c, NULL);

    _timer_t *timer = NULL;
    hy_u32_t slot_num;
    hy_u32_t slot;

    timer = HY_MEM_MALLOC_RET_VAL(_timer_t *, sizeof(*timer), NULL);
    HY_MEMCPY(&timer->timer_c, timer_c, sizeof(*timer_c));

    slot_num        = context.slot_num;
    timer->rotation = timer_c->expires / slot_num;      // 圈数
    slot            = timer_c->expires % slot_num;      // 落在哪个格子

    LOGD("rotation: %d, slot: %d \n", timer->rotation, slot);

    HyThreadMutexLock_m(context.mutex_h);
    hy_list_add_tail(&timer->list, &context.list_head[slot]);
    HyThreadMutexUnLock(context.mutex_h);

    return timer;
}

void HyTimerDel(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _timer_t *pos, *n;
    hy_u32_t i;

    for (i = 0; i < context.slot_num; ++i) {
        HyThreadMutexLock_m(context.mutex_h);

        hy_list_for_each_entry_safe(pos, n, &context.list_head[i], list) {
            if (*handle == pos) {
                hy_list_del(&pos->list);

                HY_MEM_FREE_PP(&pos);
                *handle = NULL;

                HyThreadMutexUnLock(context.mutex_h);
                goto DEL_ERR_1;
            }
        }

        HyThreadMutexUnLock(context.mutex_h);
    }

DEL_ERR_1:
    return;
}

static hy_s32_t _timer_thread_cb(void *args)
{
    struct timeval tv;
    hy_s32_t err;
    _timer_t *pos, *n;
    time_t sec, usec;
    HyTimerConfig_s *timer_c = NULL;
    hy_u32_t slot_num = context.slot_num;

    sec = context.slot_interval_ms / 1000;
    usec = (context.slot_interval_ms % 1000) * 1000;

    while (!context.exit_flag) {
        tv.tv_sec   = sec;
        tv.tv_usec  = usec;

        do {
            err = select(0, NULL, NULL, NULL, &tv);
        } while(err < 0 && errno == EINTR);

        HyThreadMutexLock_m(context.mutex_h);
        hy_list_for_each_entry_safe(pos, n, &context.list_head[context.cur_slot], list) {
            if (pos->rotation > 0) {
                pos->rotation--;
            } else {
                timer_c = &pos->timer_c;

                if (timer_c->timer_cb) {
                    timer_c->timer_cb(timer_c->args);
                }

                hy_list_del(&pos->list);

                if (pos->timer_c.mode == HY_TIMER_MODE_REPEAT) {
                    pos->rotation = timer_c->expires / slot_num;
                    hy_u32_t slot     = timer_c->expires % slot_num;
                    slot += context.cur_slot;
                    slot %= slot_num;

                    hy_list_add_tail(&pos->list, &context.list_head[slot]);
                } else {
                    HY_MEM_FREE_PP(&pos);
                }
            }
        }
        HyThreadMutexUnLock(context.mutex_h);

        context.cur_slot++;
        context.cur_slot %= slot_num;
    }

    return -1;
}

void HyTimerDestroy(void)
{
    context.exit_flag = 1;
    HyThreadDestroy(&context.thread_h);

    _timer_t *pos, *n;
    for (hy_u32_t i = 0; i < context.slot_num; ++i) {
        HyThreadMutexLock_m(context.mutex_h);

        hy_list_for_each_entry_safe(pos, n, &context.list_head[i], list) {
            hy_list_del(&pos->list);

            HY_MEM_FREE_PP(&pos);
        }

        HyThreadMutexUnLock(context.mutex_h);
    }

    HyThreadMutexDestroy(&context.mutex_h);

    HY_MEM_FREE_PP(&context.list_head);

    _is_init = 0;
    LOGI("timer destroy \n");
}

void HyTimerCreate(hy_u32_t slot_interval_ms, hy_u32_t slot_num)
{
    if (_is_init) {
        LOGW("There is no need to initialize the timer repeatedly \n");
        return;
    }

    do {
        context.slot_interval_ms    = slot_interval_ms;
        context.slot_num            = slot_num;

        context.list_head = HY_MEM_MALLOC_BREAK(struct hy_list_head *,
                sizeof(struct hy_list_head) * context.slot_num);

        for (hy_u32_t i = 0; i < context.slot_num; ++i) {
            HY_INIT_LIST_HEAD(&context.list_head[i]);
        }

        context.mutex_h = HyThreadMutexCreate_m();
        if (!context.mutex_h) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        context.thread_h = HyThreadCreate_m("HY_timer", _timer_thread_cb, NULL);
        if (!context.thread_h) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        _is_init = 1;
        LOGI("timer create \n");
        return;
    } while (0);

    LOGE("timer create failed \n");
    HyTimerDestroy();
}

