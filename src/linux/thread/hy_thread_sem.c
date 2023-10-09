/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_thread_sem.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    31/03 2022 08:48
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        31/03 2022      create the file
 * 
 *     last modified: 31/03 2022 08:48
 */
#include <stdio.h>
#include <semaphore.h>

#include "hy_assert.h"
#include "hy_mem.h"

#include "hy_thread_sem.h"

struct HyThreadSem_s {
    sem_t sem;
};

hy_s32_t HyThreadSemPost(HyThreadSem_s *handle)
{
    HY_ASSERT(handle);

    return sem_post(&handle->sem);
}

hy_s32_t HyThreadSemWait(HyThreadSem_s *handle)
{
    HY_ASSERT(handle);

    return sem_wait(&handle->sem);
}

void HyThreadSemDestroy(HyThreadSem_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);

    HyThreadSem_s *handle = *handle_pp;

    if (0 != sem_destroy(&handle->sem)) {
        LOGES("sem_destroy failed \n");
    }

    LOGI("thread sem destroy, context: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyThreadSem_s *HyThreadSemCreate(HyThreadSemConfig_s *sem_c)
{
    HY_ASSERT_RET_VAL(!sem_c, NULL);
    HyThreadSem_s *handle = NULL;

    do {
        handle = HY_MEM_MALLOC_BREAK(HyThreadSem_s *, sizeof(*handle));

        if (0 != sem_init(&handle->sem, 0, sem_c->value)) {
            LOGES("sem_init failed \n");
            break;
        }

        LOGI("thread sem create, context: %p \n", handle);
        return handle;
    } while (0);

    LOGE("thread sem create failed \n");
    HyThreadSemDestroy(&handle);
    return NULL;
}
