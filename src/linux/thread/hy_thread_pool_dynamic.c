/**
 * 
 * Copyright (C), 2010-2011,江西科萌科技控股有限公司
 * 
 * @file    hy_thread_pool_dynamic.c
 * @brief   
 * @author  zhenquan.qiu
 * @date    14/08 2023 14:59
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        14/08 2023      create the file
 * 
 *     last modified: 14/08 2023 14:59
 */
#include <stdio.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_queue.h"
#include "hy_thread_mutex.h"
#include "hy_thread.h"

#include "hy_thread_pool_dynamic.h"

#define NUMBER 2

typedef struct {
    HyThread_s                          *worker_thread_h;
    hy_s32_t                            flag;
} _worker_thread_s;

struct HyThreadPoolDynamic_s {
    HyThreadPoolDynamicSaveConfig_s     save_c;
    hy_s32_t                            is_exit;

    HyThreadMutex_s                     *mutex_h;
    HyQueue_s                           *queue_h;
    HyThread_s                          *manager_thread_h;
    _worker_thread_s                    *worker_thread;
    hy_s32_t                            live_num;
    hy_s32_t                            exit_num;

    HyThreadMutex_s                     *busy_mutex_h;
    hy_s32_t                            busy_num;
};

static void _thread_exit(HyThreadPoolDynamic_s* handle)
{
    _worker_thread_s *worker_thread;
    pthread_t tid = pthread_self();

    for (hy_s32_t i = 0; i < handle->save_c.thread_cnt_max; ++i) {
        worker_thread = &handle->worker_thread[i];

        if (HyThreadGetId(worker_thread->worker_thread_h) == tid) {
            worker_thread->flag = 0;

            LOGI("thread %ld exiting \n", tid);
            break;
        }
    }
}

static hy_s32_t _worker_loop_cb(void* args)
{
    HyThreadPoolDynamic_s *handle = (HyThreadPoolDynamic_s*)args;
    HyThreadPoolDynamicSaveConfig_s *save_c = &handle->save_c;
    void *run_befor_cb_args = NULL;
    HyThreadPoolTask_s task;

    if (save_c->run_after_cb) {
        run_befor_cb_args = save_c->run_befor_cb(save_c->run_befor_args);
        if (!run_befor_cb_args) {
            LOGE("run_befor_cb failed \n");

            _thread_exit(handle);
            return -1;
        }
    }

    while (!handle->is_exit) {
        HyThreadMutexLock(handle->mutex_h);
        if (handle->exit_num > 0) {
            handle->exit_num--;

            if (handle->live_num > save_c->thread_cnt_min) {
                handle->live_num--;
                HyThreadMutexUnLock(handle->mutex_h);

                LOGI("thread exit \n");
                break;
            }
        }
        HyThreadMutexUnLock(handle->mutex_h);

        if (0 != HyQueueRead(handle->queue_h, &task, sizeof(task))) {
            LOGI("queue read failed \n");
            continue;
        }

        HyThreadMutexLock(handle->busy_mutex_h);
        handle->busy_num++;
        HyThreadMutexUnLock(handle->busy_mutex_h);

        task.task_cb(task.args, run_befor_cb_args);

        HyThreadMutexLock(handle->busy_mutex_h);
        handle->busy_num--;
        HyThreadMutexUnLock(handle->busy_mutex_h);
    }

    if (save_c->run_after_cb) {
        save_c->run_after_cb(run_befor_cb_args);
    }

    _thread_exit(handle);

    return -1;
}

static hy_s32_t _manager_loop_cb(void* arg)
{
    HyThreadPoolDynamic_s* handle = (HyThreadPoolDynamic_s*)arg;
    HyThreadPoolDynamicSaveConfig_s *save_c = &handle->save_c;
    hy_s32_t queue_size = 0;
    hy_s32_t liveNum = 0;
    hy_s32_t busyNum = 0;
    hy_s32_t counter = 0;
    _worker_thread_s *worker_thread;
    char name[HY_THREAD_NAME_LEN_MAX];

    while (!handle->is_exit) {
        sleep(3);

        if (handle->is_exit) {
            break;
        }

        queue_size = HyQueueLenGet(handle->queue_h);

        HyThreadMutexLock(handle->mutex_h);
        liveNum = handle->live_num;
        HyThreadMutexUnLock(handle->mutex_h);

        HyThreadMutexLock(handle->busy_mutex_h);
        busyNum = handle->busy_num;
        HyThreadMutexUnLock(handle->busy_mutex_h);

        if (queue_size > liveNum && liveNum < save_c->thread_cnt_max) {
            HyThreadMutexLock(handle->mutex_h);
            counter = 0;
            for (hy_s32_t i = 0; i < save_c->thread_cnt_max && counter < NUMBER && handle->live_num < save_c->thread_cnt_max; ++i) {
                worker_thread = &handle->worker_thread[i];

                HY_MEMSET(name, sizeof(name));
                snprintf(name, sizeof(name), "threadP_worker%d", i);

                if (worker_thread->flag == 0 && !worker_thread->worker_thread_h) {
                    LOGI("create worker thread \n");

                    worker_thread->worker_thread_h = HyThreadCreate_m(name, _worker_loop_cb, handle);
                    if (!worker_thread->worker_thread_h) {
                        LOGE("HyThreadCreate_m failed \n");
                        break;
                    }
                    worker_thread->flag = 1;
                    counter++;
                    handle->live_num++;
                }
            }
            HyThreadMutexUnLock(handle->mutex_h);
        }

        if (busyNum * 2 < liveNum && liveNum > save_c->thread_cnt_min) {
            LOGI("destroy worker thread \n");

            HyThreadMutexLock(handle->mutex_h);
            handle->exit_num = NUMBER;
            HyThreadMutexUnLock(handle->mutex_h);

            for (hy_s32_t i = 0; i < NUMBER; ++i) {
                HyQueueWakeup(handle->queue_h);

                for (hy_s32_t j = 0; j < save_c->thread_cnt_max; j++) {
                    worker_thread = &handle->worker_thread[j];

                    if (worker_thread->flag == 0 && worker_thread->worker_thread_h) {
                        HyThreadDestroy(&worker_thread->worker_thread_h);
                    }
                }
            }
        }
    }

    return -1;
}

void HyThreadPoolDynamicAddTask(HyThreadPoolDynamic_s* handle, HyThreadPoolTask_s *task)
{
    HY_ASSERT(handle);
    HY_ASSERT(task);

    HyQueueWrite(handle->queue_h, task, sizeof(*task));
}

hy_s32_t HyThreadPoolDynamicGetBusyNum(HyThreadPoolDynamic_s* handle)
{
    HY_ASSERT(handle);

    hy_s32_t num = 0;

    HyThreadMutexLock(handle->busy_mutex_h);
    num = handle->busy_num;
    HyThreadMutexUnLock(handle->busy_mutex_h);

    return num;
}

hy_s32_t HyThreadPoolDynamicGetAliveNum(HyThreadPoolDynamic_s* handle)
{
    HY_ASSERT(handle);

    hy_s32_t num = 0;

    HyThreadMutexLock(handle->mutex_h);
    num = handle->live_num;
    HyThreadMutexUnLock(handle->mutex_h);

    return num;
}

void HyThreadPoolDynamicDestroy(HyThreadPoolDynamic_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);

    HyThreadPoolDynamic_s *handle = *handle_pp;
    _worker_thread_s *worker_thread;
    HyThreadPoolDynamicSaveConfig_s *save_c = &handle->save_c;

    handle->is_exit = 1;
    HyThreadDestroy(&handle->manager_thread_h);

    hy_s32_t thread_min = save_c->thread_cnt_min;
    save_c->thread_cnt_min = 0; // 等待所有任务都执行完
    for (hy_s32_t i = 0; i < thread_min; ++i) {
        HyQueueWakeup(handle->queue_h);
    }

    for (hy_s32_t i = 0; i < save_c->thread_cnt_max; i++) {
        worker_thread = &handle->worker_thread[i];

        if (worker_thread->worker_thread_h) {
            HyThreadDestroy(&worker_thread->worker_thread_h);
        }
    }

    HY_MEM_FREE_PP(&handle->worker_thread);

    HyThreadMutexDestroy(&handle->mutex_h);
    HyThreadMutexDestroy(&handle->busy_mutex_h);

    HyQueueDestroy(&handle->queue_h);

    LOGI("thread_pool destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(&handle);
}

HyThreadPoolDynamic_s *HyThreadPoolDynamicCreate(HyThreadPoolDynamicConfig_s *thread_pool_c)
{
    HY_ASSERT_RET_VAL(!thread_pool_c, NULL);

    HyThreadPoolDynamic_s* handle = NULL;
    HyThreadPoolDynamicSaveConfig_s *save_c;
    _worker_thread_s *worker_thread;
    char name[HY_THREAD_NAME_LEN_MAX];

    do {
        save_c = &thread_pool_c->save_c;

        handle = HY_MEM_MALLOC_BREAK(HyThreadPoolDynamic_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, save_c, sizeof(handle->save_c));

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

        handle->queue_h = HyQueueCreate_m(thread_pool_c->task_item_cnt * sizeof(HyThreadPoolTask_s));
        if (!handle->queue_h) {
            LOGE("HyQueueCreate failed \n");
            break;
        }

        handle->manager_thread_h = HyThreadCreate_m("threadP_manager", _manager_loop_cb, handle);
        if (!handle->manager_thread_h) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        handle->worker_thread = HY_MEM_CALLOC_BREAK(_worker_thread_s *, sizeof(_worker_thread_s) * save_c->thread_cnt_max);
        for (hy_s32_t i = 0; i < save_c->thread_cnt_min; ++i) {
            worker_thread = &handle->worker_thread[i];

            HY_MEMSET(name, sizeof(name));
            snprintf(name, sizeof(name), "threadP_worker%d", i);

            worker_thread->worker_thread_h = HyThreadCreate_m(name, _worker_loop_cb, handle);
            if (!worker_thread->worker_thread_h) {
                LOGE("HyThreadCreate_m failed \n");
                break;
            }
            worker_thread->flag = 1;
        }

        handle->live_num = save_c->thread_cnt_min;
        handle->busy_num = 0;
        handle->exit_num = 0;

        LOGI("thread_pool create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("thread_pool create failed \n");
    HyThreadPoolDynamicDestroy(&handle);
    return NULL;
}
