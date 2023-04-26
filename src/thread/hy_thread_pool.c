/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_thread_pool.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    24/04 2023 10:51
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        24/04 2023      create the file
 * 
 *     last modified: 24/04 2023 10:51
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "hy_type.h"
#include "hy_mem.h"
#include "hy_queue.h"
#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_thread_mutex.h"
#include "hy_thread.h"

#include "hy_thread_pool.h"

#define NUMBER 2

typedef struct {
    pthread_t threadIDs;
    hy_s32_t flag;
} _thread_s;

struct HyThreadPools_s {
    HyThreadPoolSaveConfig_s save_c;

    HyQueue_s *queue_h;
    pthread_t managerID;

    HyThreadMutex_s *mutex_h;
    _thread_s *worker_thread;
    hy_s32_t live_num;
    hy_s32_t exit_num;

    HyThreadMutex_s *busy_mutex_h;
    hy_s32_t busy_num;

    hy_s32_t is_exit;
};

hy_s32_t HyThreadPoolGetBusyNum(HyThreadPools_s* handle)
{
    hy_s32_t num = 0;

    HyThreadMutexLock(handle->busy_mutex_h);
    num = handle->busy_num;
    HyThreadMutexUnLock(handle->busy_mutex_h);

    return num;
}

hy_s32_t HyThreadPoolGetAliveNum(HyThreadPools_s* handle)
{
    hy_s32_t num = 0;

    HyThreadMutexLock(handle->mutex_h);
    num = handle->live_num;
    HyThreadMutexUnLock(handle->mutex_h);

    return num;
}

void HyThreadPoolAddTask(HyThreadPools_s* handle, HyThreadPoolsTask_s *task)
{
    HyQueueWrite(handle->queue_h, task);
}

static void _thread_exit(HyThreadPools_s* handle)
{
    pthread_t tid = pthread_self();

    for (hy_s32_t i = 0; i < handle->save_c.thread_cnt_max; ++i) {
        if (handle->worker_thread[i].threadIDs == tid) {
            handle->worker_thread[i].flag = 0;

            LOGI("thread %ld exiting \n", tid);
            break;
        }
    }

    pthread_exit(NULL);
}

static void *_worker_loop_cb(void* args)
{
    HyThreadPools_s *handle = (HyThreadPools_s*)args;
    HyThreadPoolsTask_s task;

    while (!handle->is_exit) {
        HyThreadMutexLock(handle->mutex_h);
        if (handle->exit_num > 0) {
            handle->exit_num--;

            if (handle->live_num > handle->save_c.thread_cnt_min) {
                handle->live_num--;
                HyThreadMutexUnLock(handle->mutex_h);

                _thread_exit(handle);
            }
        }
        HyThreadMutexUnLock(handle->mutex_h);

        if (0 != HyQueueRead(handle->queue_h, &task)) {
            continue;
        }

        HyThreadMutexLock(handle->busy_mutex_h);
        handle->busy_num++;
        HyThreadMutexUnLock(handle->busy_mutex_h);

        task.task_cb(&task);

        HyThreadMutexLock(handle->busy_mutex_h);
        handle->busy_num--;
        HyThreadMutexUnLock(handle->busy_mutex_h);
    }

    _thread_exit(handle);

    return NULL;
}

static void *manager(void* arg)
{
    HyThreadPools_s* handle = (HyThreadPools_s*)arg;
    hy_s32_t queue_size = 0;
    hy_s32_t liveNum = 0;
    hy_s32_t busyNum = 0;
    hy_s32_t counter = 0;

    while (!handle->is_exit) {
        sleep(3);

        if (handle->is_exit) {
            break;
        }

        queue_size = HyQueueGetItemCount(handle->queue_h);

        HyThreadMutexLock(handle->mutex_h);
        liveNum = handle->live_num;
        HyThreadMutexUnLock(handle->mutex_h);

        HyThreadMutexLock(handle->busy_mutex_h);
        busyNum = handle->busy_num;
        HyThreadMutexUnLock(handle->busy_mutex_h);

        if (queue_size > liveNum && liveNum < handle->save_c.thread_cnt_max) {
            HyThreadMutexLock(handle->mutex_h);
            counter = 0;
            for (hy_s32_t i = 0;
                 i < handle->save_c.thread_cnt_max
                                && counter < NUMBER
                                && handle->live_num < handle->save_c.thread_cnt_max;
                 ++i) {
                if (handle->worker_thread[i].flag == 0
                    && handle->worker_thread[i].threadIDs == 0) {
                    LOGI("create worker thread \n");

                    pthread_create(&handle->worker_thread[i].threadIDs,
                                   NULL, _worker_loop_cb, handle);
                    handle->worker_thread[i].flag = 1;
                    counter++;
                    handle->live_num++;
                }
            }
            HyThreadMutexUnLock(handle->mutex_h);
        }

        if (busyNum * 2 < liveNum && liveNum > handle->save_c.thread_cnt_min) {
            LOGI("destroy worker thread \n");

            HyThreadMutexLock(handle->mutex_h);
            handle->exit_num = NUMBER;
            HyThreadMutexUnLock(handle->mutex_h);

            for (hy_s32_t i = 0; i < NUMBER; ++i) {
                HyQueueWakeup(handle->queue_h);

                for (hy_s32_t j = 0; j < handle->save_c.thread_cnt_max; j++) {
                    if (handle->worker_thread[j].flag == 0
                        && handle->worker_thread[j].threadIDs) {
                        pthread_join(handle->worker_thread[j].threadIDs, NULL);
                        handle->worker_thread[j].threadIDs = 0;
                    }
                }
            }
        }
    }

    return NULL;
}

void HyThreadPoolDestroy(HyThreadPools_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyThreadPools_s *handle = *handle_pp;

    handle->is_exit = 1;
    pthread_join(handle->managerID, NULL);

    hy_s32_t thread_min = handle->save_c.thread_cnt_min;
    handle->save_c.thread_cnt_min = 0;
    for (hy_s32_t i = 0; i < thread_min; ++i) {
        HyQueueWakeup(handle->queue_h);
    }
    sleep(1);

    for (hy_s32_t i = 0; i < handle->save_c.thread_cnt_max; i++) {
        if (handle->worker_thread[i].threadIDs) {
            pthread_join(handle->worker_thread[i].threadIDs, NULL);
        }
    }

    HY_MEM_FREE_PP(&handle->worker_thread);

    HyThreadMutexDestroy(&handle->mutex_h);
    HyThreadMutexDestroy(&handle->busy_mutex_h);

    HyQueueDestroy(&handle->queue_h);

    LOGI("thread_pool destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(&handle);
}

HyThreadPools_s *HyThreadPoolCreate(HyThreadPoolConfig_s *thread_pool_c)
{
    HY_ASSERT_RET_VAL(!thread_pool_c, NULL);
    HyThreadPools_s* handle = NULL;

    do {
        handle = HY_MEM_MALLOC_BREAK(HyThreadPools_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &thread_pool_c->save_c, sizeof(handle->save_c));

        handle->mutex_h = HyThreadMutexCreate_m();
        if (!handle->mutex_h) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        handle->busy_mutex_h = HyThreadMutexCreate_m();
        if (!handle->busy_mutex_h) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        handle->queue_h = HyQueueCreate_m(thread_pool_c->task_item_cnt,
                                          thread_pool_c->task_item_len);
        if (!handle->queue_h) {
            LOGE("HyQueueCreate failed \n");
            break;
        }

        pthread_create(&handle->managerID, NULL, manager, handle);

        handle->worker_thread
            = HY_MEM_MALLOC_BREAK(_thread_s *,
                                  sizeof(_thread_s) * thread_pool_c->save_c.thread_cnt_max);
        for (hy_s32_t i = 0; i < thread_pool_c->save_c.thread_cnt_min; ++i) {
            pthread_create(&handle->worker_thread[i].threadIDs,
                           NULL, _worker_loop_cb, handle);
            handle->worker_thread[i].flag = 1;
        }

        handle->live_num = thread_pool_c->save_c.thread_cnt_min;
        handle->busy_num = 0;
        handle->exit_num = 0;

        LOGI("thread_pool create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGI("thread_pool create failed \n");
    return NULL;
}
