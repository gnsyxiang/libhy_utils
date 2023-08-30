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
#include <stdio.h>

#include <hy_log/hy_log.h>
#include <unistd.h>

#include "hy_mem.h"
#include "hy_hex.h"
#include "hy_string.h"
#include "hy_fifo_lock.h"
#include "hy_thread.h"
#include "hy_thread_mutex.h"

#include "hy_thread_pool.h"

struct HyThreadPool_s {
    HyThreadPoolSaveConfig_s    save_c;
    hy_s32_t                    is_exit;

    HyFifoLock_s                *fifo_lock_h;
    HyThread_s                  **worker_thread_h;
};

static hy_s32_t _worker_loop_cb(void *args)
{
    HyThreadPool_s *handle = (HyThreadPool_s*)args;
    HyThreadPoolSaveConfig_s *save_c = &handle->save_c;
    void *run_befor_cb_args = NULL;
    HyThreadPoolTask_s task;

    if (save_c->run_after_cb) {
        run_befor_cb_args = save_c->run_befor_cb(save_c->run_befor_args);
        if (!run_befor_cb_args) {
            LOGE("run_befor_cb failed \n");
            return -1;
        }
    }

    while (!handle->is_exit) {
        if (HyFifoLockRead(handle->fifo_lock_h, &task, sizeof(task)) == 0) {
            LOGD("HyFifoLockRead is failed \n");

            break;
        }

        task.task_cb(task.args, run_befor_cb_args);
    }

    if (save_c->run_after_cb) {
        save_c->run_after_cb(run_befor_cb_args);
    }

    return -1;
}

void HyThreadPoolAddTask(HyThreadPool_s *handle, HyThreadPoolTask_s *task)
{
    HY_ASSERT(handle);
    HY_ASSERT(task);

    HyFifoLockWrite(handle->fifo_lock_h, task, sizeof(*task));
}

void HyThreadPoolDestroy(HyThreadPool_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);

    HyThreadPool_s *handle = *handle_pp;
    HyThreadPoolSaveConfig_s *save_c = &handle->save_c;

    handle->is_exit = 1;

    HyFifoLockDestroy(&handle->fifo_lock_h);

    usleep(1 * 1000);

    for (hy_s32_t i = 0; i < save_c->thread_cnt; ++i) {
        HyThreadDestroy(&handle->worker_thread_h[i]);
    }

    HY_MEM_FREE_PP(&handle->worker_thread_h);

    LOGI("thread_pool destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(&handle);
}

HyThreadPool_s *HyThreadPoolCreate(HyThreadPoolConfig_s *thread_pool_c)
{
    HY_ASSERT_RET_VAL(!thread_pool_c, NULL);

    HyThreadPool_s* handle = NULL;
    HyThreadPoolSaveConfig_s *save_c;
    char name[HY_THREAD_NAME_LEN_MAX];

    do {
        save_c = &thread_pool_c->save_c;

        handle = HY_MEM_MALLOC_BREAK(HyThreadPool_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, save_c, sizeof(handle->save_c));

        handle->fifo_lock_h = HyFifoLockCreate_m(thread_pool_c->task_item_cnt * sizeof(HyThreadPoolTask_s));
        if (!handle->fifo_lock_h) {
            LOGE("HyFifoLockCreate_m failed \n");
            break;
        }

        handle->worker_thread_h = HY_MEM_CALLOC_BREAK(HyThread_s **, sizeof(HyThread_s *) * save_c->thread_cnt);
        for (hy_s32_t i = 0; i < save_c->thread_cnt; ++i) {
            HY_MEMSET(name, sizeof(name));
            snprintf(name, sizeof(name), "ThrdP_Worker%d", i);

            handle->worker_thread_h[i] = HyThreadCreate_m(name, _worker_loop_cb, handle);
            if (!handle->worker_thread_h[i]) {
                LOGE("HyThreadCreate_m failed \n");
                break;
            }
        }

        LOGI("thread_pool create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("thread_pool create failed \n");
    HyThreadPoolDestroy(&handle);
    return NULL;
}
