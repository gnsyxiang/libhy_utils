/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_thread_pool.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    29/12 2021 15:33
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        29/12 2021      create the file
 * 
 *     last modified: 29/12 2021 15:33
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_log.h"

#include "hy_utils.h"

#include "hy_thread_pool.h"

typedef struct {
    HyThreadPoolTaskCb_t    task_cb;
    void                    *args;
} _task_t;

typedef struct {
    hy_s32_t            shutdown_flag;
    hy_s32_t            shutdown;
    pthread_cond_t      cond;
    pthread_mutex_t     mutex;

    hy_u32_t            thread_max_cnt;
    hy_u32_t            thread_live_cnt;
    void                **thread_handle;

    _task_t             *tasks;
    hy_u32_t            task_max_cnt;
    hy_u32_t            task_pending_cnt;
    hy_u32_t            read_pos;
    hy_u32_t            write_pos;
} _thread_pool_context_t;

hy_s32_t HyThreadPoolAdd(void *handle, HyThreadPoolTaskCb_t task_cb, void *args)
{
    LOGT("handle: %p, task_cb: %p, args: %p \n", handle, task_cb, args);
    HY_ASSERT_VAL_RET_VAL(!handle || !task_cb, HY_THREAD_POOL_INVALID);

    _thread_pool_context_t *context = handle;
    hy_s32_t ret = HY_THREAD_POOL_OK;
    hy_u32_t next = 0;

    if (0 != pthread_mutex_lock(&context->mutex)) {
        LOGE("mutex lock failed \n");
        return HY_THREAD_POOL_SYSCALL_FAILD;
    }

    next = (context->write_pos + 1) & (context->task_max_cnt - 1);

    do {
        if (context->task_pending_cnt == context->task_max_cnt) {
            LOGE("task full \n");
            ret = HY_THREAD_POOL_TASK_FULL;
            break;
        }

        if (context->shutdown) {
            LOGE("shut down \n");
            ret = HY_THREAD_POOL_SHUT_DOWN;
            break;
        }

        context->tasks[context->write_pos].task_cb  = task_cb;
        context->tasks[context->write_pos].args     = args;
        context->write_pos = next;
        context->task_pending_cnt++;

        if (0 != pthread_cond_signal(&context->cond)) {
            LOGE("cond signal failed \n");
            ret = HY_THREAD_POOL_SYSCALL_FAILD;
            break;
        }
    } while (0);

    if (0 != pthread_mutex_unlock(&context->mutex)) {
        LOGE("mutex unlock failed \n");
        ret = HY_THREAD_POOL_SYSCALL_FAILD;
    }

    return ret;
}

static hy_s32_t _thread_pool_loop_cb(void *args)
{
    _thread_pool_context_t *context = args;
    _task_t task;
    hy_u32_t next;
    LOGI("thread pool start, handle: %p \n", context);

    while (1) {
        pthread_mutex_lock(&context->mutex);

        next = (context->read_pos + 1) & (context->task_max_cnt - 1);

        while (context->task_pending_cnt == 0
                && context->shutdown == HY_THREAD_POOL_DESTROY_RUNNING) {
            LOGD("wait cond \n");
            pthread_cond_wait(&context->cond, &context->mutex);
        }

        if (context->shutdown == HY_THREAD_POOL_DESTROY_IMMEDIATE
                || (context->shutdown == HY_THREAD_POOL_DESTROY_GRACEFUL
                    && context->task_pending_cnt == 0)) {
            LOGD("thread pool loop break \n");
            break;
        }

        task.task_cb        = context->tasks[context->read_pos].task_cb;
        task.args           = context->tasks[context->read_pos].args;
        context->read_pos   = next;
        context->task_pending_cnt--;

        pthread_mutex_unlock(&context->mutex);

        task.task_cb(task.args);
    }

    context->thread_live_cnt--;
    pthread_mutex_unlock(&context->mutex);

    LOGI("thread pool stop, handle: %p, id: %lx \n", context, pthread_self());
    return -1;
}

void HyThreadPoolDestroy(void **handle)
{
    LOGT("handle: %p, *handle: %p \n", handle, *handle);
    HY_ASSERT_VAL_RET(!handle || !*handle);

    _thread_pool_context_t *context = *handle;

    context->shutdown = context->shutdown_flag;

    pthread_mutex_lock(&context->mutex);
    if (0 != (pthread_cond_broadcast(&context->cond))) {
        LOGE("broad cast signal failed \n");
        pthread_mutex_unlock(&context->mutex);
        return;
    }
    pthread_mutex_unlock(&context->mutex);

    for (hy_u32_t i = 0; i < context->thread_max_cnt; i++) {
        HyThreadDestroy(&context->thread_handle[i]);
    }

    HY_MEM_FREE_PP(&context->thread_handle);
    HY_MEM_FREE_PP(&context->tasks);

    pthread_mutex_destroy(&context->mutex);
    pthread_cond_destroy(&context->cond);

    LOGI("thread pool destroy, handle: %p \n", context);
    HY_MEM_FREE_PP(&context);
}

void *HyThreadPoolCreate(HyThreadPoolConfig_t *config)
{
    LOGT("config: %p \n", config);
    HY_ASSERT_VAL_RET_VAL(!config, NULL);

    _thread_pool_context_t *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_thread_pool_context_t *, sizeof(*context));

        if (!HY_UTILS_IS_POWER_OF_2(config->task_max_cnt)) {
            config->task_max_cnt = HyUtilsNumTo2N(config->task_max_cnt);

            LOGW("size must be power of 2, new size: %d \n",
                    config->task_max_cnt);
        }

        if (0 != (pthread_mutex_init(&context->mutex, NULL))) {
            LOGE("mutex init failed \n");
            break;
        }

        if (0 != (pthread_cond_init(&context->cond, NULL))) {
            LOGE("cond init failed \n");
            break;
        }

        context->shutdown_flag = config->shutdown_flag;

        context->task_max_cnt = config->task_max_cnt;
        context->write_pos = context->read_pos = 0;
        context->tasks = HY_MEM_MALLOC_BREAK(_task_t *,
                sizeof(_task_t) * (config->task_max_cnt));

        context->thread_max_cnt = 0;
        context->thread_live_cnt = 0;
        context->thread_handle = HY_MEM_MALLOC_BREAK(void *,
                sizeof(void *) * config->thread_max_cnt);

        for (hy_u32_t i = 0; i < config->thread_max_cnt; ++i) {
            char thread_name[16] = {0};
            snprintf(thread_name, sizeof(thread_name), "hy_thd_pool_%d", i);

            context->thread_handle[i] = HyThreadCreate_m(thread_name,
                    _thread_pool_loop_cb, HY_THREAD_DESTROY_GRACE, context);
            if (!context->thread_handle[i]) {
                LOGE("create thread failed \n");
                goto _ERR_THREAD_POOL_CREATE_1;
            }

            context->thread_live_cnt++;
            context->thread_max_cnt++;
        }

        LOGI("thread pool create, handle: %p \n", context);
        return context;
    } while (0);

_ERR_THREAD_POOL_CREATE_1:
    HyThreadPoolDestroy((void **)&context);
    return NULL;
}
