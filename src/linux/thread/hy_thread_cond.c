/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_thread_cond.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    31/03 2022 09:39
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        31/03 2022      create the file
 * 
 *     last modified: 31/03 2022 09:39
 */
#include <stdio.h>

#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_time.h"
#include "hy_thread_mutex.h"
#include "hy_thread_cond.h"

hy_s32_t HyThreadCondWait(HyThreadCond_s *handle, HyThreadMutex_s *mutex_h, hy_u32_t timeout_ms)
{
    HY_ASSERT(handle);
    HY_ASSERT(mutex_h);

    hy_s32_t ret = 0;

    // @fixme
    pthread_mutex_t *mutex = HyThreadMutexGetLock(mutex_h);

    if (timeout_ms == 0) {
        ret = pthread_cond_wait(&handle->cond, mutex);
    } else {
        struct timespec ts = HyTimeGetTimespec(timeout_ms);
        ret = pthread_cond_timedwait(&handle->cond, mutex, &ts);
    }

    return ret == 0 ? 0 : -1;
}

void HyThreadCondDestroy(HyThreadCond_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyThreadCond_s *handle = *handle_pp;

    if (0 != pthread_cond_destroy(&handle->cond)) {
        LOGES("pthread_cond_destroy faield \n");
    }

    LOGI("thread cond destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyThreadCond_s *HyThreadCondCreate(HyThreadCondConfig_s *cond_c)
{
    HY_ASSERT_RET_VAL(!cond_c, NULL);
    HyThreadCond_s *handle = NULL;

    do {
        handle = HY_MEM_MALLOC_BREAK(HyThreadCond_s *, sizeof(*handle));

        if (0 != pthread_cond_init(&handle->cond, NULL)) {
            LOGES("pthread_cond_init failed \n");
            break;
        }

        LOGI("thread cond create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("thread cond create failed \n");
    HyThreadCondDestroy(&handle);
    return NULL;
}
