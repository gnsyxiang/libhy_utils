/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_thread_mutex.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 19:46
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 19:46
 */
#include <stdio.h>
#include <pthread.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_mem.h"

#include "hy_thread_mutex.h"

struct HyThreadMutex_s {
    pthread_mutex_t mutex;
};

hy_s32_t HyThreadMutexTryLock(HyThreadMutex_s *handle)
{
    HY_ASSERT(handle);

    return pthread_mutex_trylock(&handle->mutex) == 0 ? 0 : -1;
}

hy_s32_t HyThreadMutexLock(HyThreadMutex_s *handle)
{
    HY_ASSERT(handle);

    return pthread_mutex_lock(&handle->mutex) == 0 ? 0 : -1;
}

hy_s32_t HyThreadMutexUnLock(HyThreadMutex_s *handle)
{
    HY_ASSERT(handle);

    return pthread_mutex_unlock(&handle->mutex) == 0 ? 0 : -1;
}

void *HyThreadMutexGetLock(HyThreadMutex_s *handle)
{
    HY_ASSERT(handle);

    return &handle->mutex;
}

void HyThreadMutexDestroy(HyThreadMutex_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyThreadMutex_s *handle = *handle_pp;

    if (0 != pthread_mutex_destroy(&handle->mutex)) {
        LOGES("pthread_mutex_destroy failed \n");
    }

    LOGI("thread mutex destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyThreadMutex_s *HyThreadMutexCreate(HyThreadMutexConfig_s *mutex_c)
{
    HY_ASSERT_RET_VAL(!mutex_c, NULL);
    HyThreadMutex_s *handle = NULL;

    do {
        handle = HY_MEM_MALLOC_BREAK(HyThreadMutex_s *, sizeof(*handle));

        if (0 != pthread_mutex_init(&handle->mutex, NULL)) {
            LOGES("pthread_mutex_init failed \n");
            break;
        }

        LOGI("thread mutex create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("thread mutex create failed \n");
    HyThreadMutexDestroy((HyThreadMutex_s **)&handle);
    return NULL;
}

