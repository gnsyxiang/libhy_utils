/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_queue.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    24/04 2023 14:30
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        24/04 2023      create the file
 * 
 *     last modified: 24/04 2023 14:30
 */
#include <stdio.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_utils.h"
#include "hy_mem.h"
#include "thread/hy_thread_mutex.h"
#include "thread/hy_thread_cond.h"

#include "hy_queue.h"

struct HyQueue_s {
    HyQueueSaveConfig_s     save_c;

    void                    *buf;
    hy_u32_t                size;
    hy_u32_t                read_pos;
    hy_u32_t                write_pos;
    HyThreadMutex_s         *mutex;
    HyThreadCond_s          *not_full;
    HyThreadCond_s          *not_empty;
};

hy_s32_t HyQueueWakeup(HyQueue_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    HyThreadCondSignal(handle->not_empty);
    return 0;
}

hy_s32_t HyQueueGetItemCount(HyQueue_s *handle)
{
    hy_u32_t cnt = 0;
    HY_ASSERT_RET_VAL(!handle, -1);

    HyThreadMutexLock(handle->mutex);
    cnt = handle->size;
    HyThreadMutexUnLock(handle->mutex);

    return cnt;
}

hy_s32_t HyQueueReadPeek(HyQueue_s *handle, void *item)
{
    HyQueueSaveConfig_s *save_c = &handle->save_c;
    hy_s32_t pos = 0;

    HyThreadMutexLock(handle->mutex);
    while (handle->size == 0) {
        HyThreadCondWait(handle->not_empty, handle->mutex, 0);
    }

    pos = handle->read_pos * save_c->item_len;
    HY_MEMCPY(item, handle->buf + pos, save_c->item_len);

    HyThreadCondSignal(handle->not_full);
    HyThreadMutexUnLock(handle->mutex);

    return 0;
}

hy_s32_t HyQueueReadDel(HyQueue_s *handle, hy_u32_t item_cnt)
{
    HyThreadMutexLock(handle->mutex);
    while (handle->size == 0) {
        HyThreadCondWait(handle->not_empty, handle->mutex, 0);
    }

    handle->read_pos = (handle->read_pos + item_cnt) & (handle->save_c.item_cnt - 1);
    handle->size -= item_cnt;

    HyThreadCondSignal(handle->not_full);
    HyThreadMutexUnLock(handle->mutex);

    return 0;
}

hy_s32_t HyQueueRead(HyQueue_s *handle, void *item)
{
    HyQueueSaveConfig_s *save_c = &handle->save_c;
    hy_s32_t pos = 0;

    HyThreadMutexLock(handle->mutex);
    while (handle->size == 0) {
        HyThreadCondWait(handle->not_empty, handle->mutex, 0);

        if (handle->size == 0) {
            HyThreadMutexUnLock(handle->mutex);
            return -1;
        }
    }

    pos = handle->read_pos * save_c->item_len;
    HY_MEMCPY(item, handle->buf + pos, save_c->item_len);
    handle->read_pos = (handle->read_pos + 1) & (handle->save_c.item_cnt - 1);
    handle->size--;

    HyThreadCondSignal(handle->not_full);
    HyThreadMutexUnLock(handle->mutex);

    return 0;
}

hy_s32_t HyQueueWrite(HyQueue_s *handle, const void *item)
{
    HY_ASSERT(handle);

    HyQueueSaveConfig_s *save_c = &handle->save_c;
    hy_s32_t pos = 0;

    HyThreadMutexLock(handle->mutex);
    while (handle->size == handle->save_c.item_cnt) {
        HyThreadCondWait(handle->not_full, handle->mutex, 0);
    }

    pos = handle->write_pos * save_c->item_len;
    HY_MEMCPY(handle->buf + pos, item, save_c->item_len);
    handle->write_pos = (handle->write_pos + 1) & (handle->save_c.item_cnt - 1);
    handle->size++;

    HyThreadCondSignal(handle->not_empty);
    HyThreadMutexUnLock(handle->mutex);

    return 0;
}

void HyQueueDestroy(HyQueue_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyQueue_s *handle = *handle_pp;

    HY_MEM_FREE_P(handle->buf);

    HyThreadCondDestroy(&handle->not_empty);
    HyThreadCondDestroy(&handle->not_full);
    HyThreadMutexDestroy(&handle->mutex);

    LOGI("queue destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyQueue_s *HyQueueCreate(HyQueueConfig_s *queue_c)
{
    HY_ASSERT_RET_VAL(!queue_c, NULL);
    HyQueue_s *handle = NULL;

    do {
        handle = HY_MEM_MALLOC_BREAK(HyQueue_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &queue_c->save_c, sizeof(queue_c->save_c));

        HyQueueSaveConfig_s  *save_c = &handle->save_c;

        if (!HY_UTILS_IS_POWER_OF_2(save_c->item_cnt)) {
            LOGW("old item_cnt: %d \n", save_c->item_cnt);
            save_c->item_cnt = HyUtilsNumTo2N(save_c->item_cnt);
            LOGW("item_cnt must be power of 2, new item_cnt: %d \n", save_c->item_cnt);
        }

        handle->buf = HY_MEM_MALLOC_BREAK(void *,
                                          save_c->item_cnt * save_c->item_len);

        handle->write_pos = handle->read_pos = 0;

        handle->mutex = HyThreadMutexCreate_m();
        if (!handle->mutex) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        handle->not_full = HyThreadCondCreate_m();
        if (!handle->not_full) {
            LOGE("HyThreadMutexCreate_m failed \n");
        }

        handle->not_empty = HyThreadCondCreate_m();
        if (!handle->not_empty) {
            LOGE("HyThreadMutexCreate_m failed \n");
        }

        LOGI("queue create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("queue create failed \n");
    HyQueueDestroy(&handle);
    return NULL;
}
