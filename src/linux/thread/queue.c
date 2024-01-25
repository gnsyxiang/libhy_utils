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

#include <hy_os_type/hy_assert.h>
#include <hy_os_type/hy_utils.h>
#include <hy_os_type/hy_mem.h>

#include <hy_os/hy_thread_mutex.h>
#include <hy_os/hy_thread_cond.h>

#include "hy_queue.h"

struct HyQueue_s {
    HyQueueSaveConfig_s     save_c;

    void                    *buf;
    hy_u32_t                len;
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

hy_s32_t HyQueueLenGet(HyQueue_s *handle)
{
    hy_u32_t cnt = 0;
    HY_ASSERT_RET_VAL(!handle, -1);

    HyThreadMutexLock(handle->mutex);
    cnt = handle->len;
    HyThreadMutexUnLock(handle->mutex);

    return cnt;
}

hy_s32_t HyQueueRemove(HyQueue_s *handle, hy_u32_t len)
{
    HyThreadMutexLock(handle->mutex);
    while (handle->len == 0) {
        HyThreadCondWait(handle->not_empty, handle->mutex, 0);
    }

    handle->read_pos = (handle->read_pos + len) & (handle->save_c.capacity - 1);
    handle->len -= len;

    HyThreadCondSignal(handle->not_full);
    HyThreadMutexUnLock(handle->mutex);

    return 0;
}

hy_s32_t HyQueueReadPeek(HyQueue_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    HyThreadMutexLock(handle->mutex);
    while (handle->len == 0) {
        HyThreadCondWait(handle->not_empty, handle->mutex, 0);
    }

    HY_MEMCPY(buf, handle->buf + handle->read_pos, len);

    HyThreadMutexUnLock(handle->mutex);

    return 0;
}

hy_s32_t HyQueueRead(HyQueue_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    HyThreadMutexLock(handle->mutex);
    while (handle->len == 0) {
        HyThreadCondWait(handle->not_empty, handle->mutex, 0);

        if (handle->len == 0) {
            HyThreadMutexUnLock(handle->mutex);
            return -1;
        }
    }

    HY_MEMCPY(buf, handle->buf + handle->read_pos, len);
    handle->read_pos = (handle->read_pos + len) & (handle->save_c.capacity - 1);
    handle->len -= len;

    HyThreadCondSignal(handle->not_full);
    HyThreadMutexUnLock(handle->mutex);

    return 0;
}

hy_s32_t HyQueueWrite(HyQueue_s *handle, const void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    HyThreadMutexLock(handle->mutex);
    while (handle->len + len > handle->save_c.capacity) {
        HyThreadCondWait(handle->not_full, handle->mutex, 0);
    }

    HY_MEMCPY(handle->buf + handle->write_pos, buf, len);
    handle->write_pos = (handle->write_pos + len) & (handle->save_c.capacity - 1);
    handle->len += len;

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
        if (!HY_UTILS_IS_POWER_OF_2(save_c->capacity)) {
            LOGW("old len: %d \n", save_c->capacity);

            save_c->capacity = HyUtilsNumTo2N(save_c->capacity);
            LOGW("len must be power of 2, new len: %d \n", save_c->capacity);
        }
        handle->buf = HY_MEM_MALLOC_BREAK(void *, save_c->capacity);

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
