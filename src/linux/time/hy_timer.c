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
#include <unistd.h>
#include <errno.h>

#include <hy_os/hy_assert.h>
#include <hy_os/hy_mem.h>
#include <hy_os/hy_string.h>
#include <hy_os/hy_list.h>

#include <hy_os/hy_thread.h>
#include <hy_os/hy_thread_mutex.h>

#include "hy_timer.h"

struct HyTimerServer_s {
    HyTimerSaveServerConfig_s   save_c;

    HyThreadMutex_s             *mutex_h;
    struct hy_list_head         *list_head;

    hy_s32_t                    is_exit;
    HyThread_s                  *thread_h;

    hy_u32_t                    cur_slot;
};

struct HyTimer_s {
    HyTimerParam_s              timer_p;

    hy_u32_t                    rotation;           ///< 旋转的圈数，类比分针走一圈

    struct hy_list_head         list;
};

static hy_s32_t _timer_server_loop_cb(void *args)
{
    HyTimerServer_s *handle = args;
    HyTimerSaveServerConfig_s *save_c = &handle->save_c;
    HyTimerParam_s *timer_c = NULL;
    HyTimer_s *pos, *n;
    struct timeval tv;
    time_t sec, usec;
    hy_s32_t err;
    hy_u32_t slot;
    hy_u32_t slot_num;

    slot_num = handle->save_c.slot_num;

    sec = save_c->slot_interval_ms / 1000;
    usec = (save_c->slot_interval_ms % 1000) * 1000;

    while (!handle->is_exit) {
        tv.tv_sec   = sec;
        tv.tv_usec  = usec;

        do {
            err = select(0, NULL, NULL, NULL, &tv);
        } while(err < 0 && errno == EINTR);

        HyThreadMutexLock_m(handle->mutex_h);
        hy_list_for_each_entry_safe(pos, n, &handle->list_head[handle->cur_slot], list) {
            if (pos->rotation > 0) {
                pos->rotation--;
            }

            if (pos->rotation == 0) {
                // 从指定链表中删除
                hy_list_del(&pos->list);

                timer_c = &pos->timer_p;
                if (timer_c->timer_cb) {
                    timer_c->timer_cb(timer_c->args);
                }

                if (pos->timer_p.mode == HY_TIMER_MODE_REPEAT) {
                    slot = handle->cur_slot + timer_c->expires;
                    pos->rotation = slot / slot_num;
                    slot %= slot_num;

                    LOGD("rotation: %d, slot: %d, cur_slot: %d \n",
                         pos->rotation, slot, handle->cur_slot);

                    // 添加到指定的链表中
                    hy_list_add_tail(&pos->list, &handle->list_head[slot]);
                } else {
                    HY_MEM_FREE_PP(&pos);
                }
            }
        }

        handle->cur_slot++;
        handle->cur_slot %= handle->save_c.slot_num;
        HyThreadMutexUnLock_m(handle->mutex_h);
    }

    return -1;
}

HyTimer_s *HyTimerAdd(HyTimerServer_s *handle, HyTimerParam_s *timer_p)
{
    HY_ASSERT_RET_VAL(!handle || !timer_p, NULL);

    HyTimer_s *timer = NULL;
    hy_u32_t slot_num;
    hy_u32_t slot;

    timer = HY_MEM_CALLOC_RETURN_VAL(HyTimer_s *, sizeof(*timer), NULL);
    HY_MEMCPY(&timer->timer_p, timer_p, sizeof(*timer_p));

    slot_num        = handle->save_c.slot_num;
    timer->rotation = timer_p->expires / slot_num;
    slot            = timer_p->expires % slot_num;

    LOGI("rotation: %d, slot: %d \n", timer->rotation, slot);

    HyThreadMutexLock_m(handle->mutex_h);
    hy_list_add_tail(&timer->list, &handle->list_head[slot]);
    HyThreadMutexUnLock(handle->mutex_h);

    LOGI("timer create, timer: %p \n", timer);
    return timer;
}

void HyTimerDel(HyTimerServer_s *handle, HyTimer_s **timer_pp)
{
    HY_ASSERT_RET(!handle || !timer_pp || !*timer_pp);
    HyTimer_s *timer = *timer_pp;
    HyTimer_s *pos, *n;

    for (hy_u32_t i = 0; i < handle->save_c.slot_num; ++i) {
        HyThreadMutexLock_m(handle->mutex_h);

        hy_list_for_each_entry_safe(pos, n, &handle->list_head[i], list) {
            if (timer == pos) {
                hy_list_del(&timer->list);
                HY_MEM_FREE_PP(timer_pp);

                HyThreadMutexUnLock(handle->mutex_h);
                return;
            }
        }

        HyThreadMutexUnLock(handle->mutex_h);
    }
}

void HyTimerServerDestroy(HyTimerServer_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyTimerServer_s *handle = *handle_pp;

    handle->is_exit = 1;
    HyThreadDestroy(&handle->thread_h);

    HyTimer_s *pos, *n;
    for (hy_u32_t i = 0; i < handle->save_c.slot_num; ++i) {
        HyThreadMutexLock_m(handle->mutex_h);

        hy_list_for_each_entry_safe(pos, n, &handle->list_head[i], list) {
            hy_list_del(&pos->list);
            HY_MEM_FREE_PP(&pos);
        }

        HyThreadMutexUnLock(handle->mutex_h);
    }

    HyThreadMutexDestroy(&handle->mutex_h);

    HY_MEM_FREE_PP(&handle->list_head);

    LOGI("timer server destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyTimerServer_s *HyTimerServerCreate(HyTimerServerConfig_s *timer_server_c)
{
    HY_ASSERT_RET_VAL(!timer_server_c, NULL);
    HyTimerServer_s *handle = NULL;
    HyTimerSaveServerConfig_s *save_c;
    hy_u32_t len;

    do {
        handle = HY_MEM_CALLOC_BREAK(HyTimerServer_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &timer_server_c->save_c, sizeof(handle->save_c));

        save_c = &handle->save_c;

        len = sizeof(struct hy_list_head) * save_c->slot_num;
        handle->list_head = HY_MEM_MALLOC_BREAK(struct hy_list_head *, len);

        for (hy_u32_t i = 0; i < save_c->slot_num; ++i) {
            HY_INIT_LIST_HEAD(&handle->list_head[i]);
        }

        handle->mutex_h = HyThreadMutexCreate_m();
        if (!handle->mutex_h) {
            LOGE("HyThreadMutexCreate_m failed, handle: %p \n", handle);
            break;
        }

        handle->thread_h = HyThreadCreate_m("hy_timer_server", _timer_server_loop_cb, handle);
        if (!handle->thread_h) {
            LOGE("HyThreadCreate_m failed, handle: %p \n", handle);
            break;
        }

        LOGI("timer server create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("timer server create failed \n");
    HyTimerServerDestroy(&handle);
    return NULL;
}
