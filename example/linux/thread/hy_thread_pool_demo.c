/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_thread_pool_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    29/12 2021 17:18
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        29/12 2021      create the file
 * 
 *     last modified: 29/12 2021 17:18
 */
#include <stdio.h>

#include <hy_log/hy_log.h>

#include "config.h"

#include "hy_type.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_signal.h"
#include "hy_module.h"
#include "hy_utils.h"
#include "hy_thread_mutex.h"

#include "hy_thread_pool.h"

#define _APP_NAME "hy_thread_pool_demo"

#define _TEST_TASK_NUM (500)

typedef struct {
    hy_s32_t        is_exit;

    HyThreadMutex_s *mutex_h;
    hy_u32_t        num;
    HyThreadPool_s  *thread_pool_h;
} _main_context_s;

static void _signal_error_cb(void *args)
{
    _main_context_s *context = args;
    context->is_exit = 1;

    LOGE("------error cb\n");
}

static void _signal_user_cb(void *args)
{
    _main_context_s *context = args;
    context->is_exit = 1;

    LOGW("------user cb\n");
}

static void _bool_module_destroy(_main_context_s **context_pp)
{
    HyModuleDestroyBool_s bool_module[] = {
        {"signal",          HySignalDestroy },
        {"log",             HyLogDeInit     },
    };

    HY_MODULE_RUN_DESTROY_BOOL(bool_module);
}

static hy_s32_t _bool_module_create(_main_context_s *context)
{
    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(log_c));
    log_c.config_file               = "../res/hy_log/zlog.conf";
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.level              = HY_LOG_LEVEL_INFO;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL_NO_PID_ID;

    int8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    int8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGINT, SIGTERM, SIGUSR1, SIGUSR2,
    };

    HySignalConfig_t signal_c;
    HY_MEMSET(&signal_c, sizeof(signal_c));
    HY_MEMCPY(signal_c.error_num, signal_error_num, sizeof(signal_error_num));
    HY_MEMCPY(signal_c.user_num, signal_user_num, sizeof(signal_user_num));
    signal_c.save_c.app_name        = _APP_NAME;
    signal_c.save_c.coredump_path   = "./";
    signal_c.save_c.error_cb        = _signal_error_cb;
    signal_c.save_c.user_cb         = _signal_user_cb;
    signal_c.save_c.args            = context;

    HyModuleCreateBool_s bool_module[] = {
        {"log",         &log_c,         (HyModuleCreateBoolCb_t)HyLogInit,          HyLogDeInit},
        {"signal",      &signal_c,      (HyModuleCreateBoolCb_t)HySignalCreate,     HySignalDestroy},
    };

    HY_MODULE_RUN_CREATE_BOOL(bool_module);
}

static void _handle_module_destroy(_main_context_s **context_pp)
{
    _main_context_s *context = *context_pp;

    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"thread_pool",     (void *)&context->thread_pool_h,    (HyModuleDestroyHandleCb_t)HyThreadPoolDestroy},
        {"mutext",          (void *)&context->mutex_h,          (HyModuleDestroyHandleCb_t)HyThreadMutexDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_s *context)
{
    HyThreadPoolConfig_s thread_pool_c;
    HY_MEMSET(&thread_pool_c, sizeof(thread_pool_c));
    thread_pool_c.task_item_cnt         = _TEST_TASK_NUM;
    thread_pool_c.save_c.thread_cnt     = 100;

    HyThreadMutexConfig_s mutex_c;
    HY_MEMSET(&mutex_c, sizeof(mutex_c));

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"mutext",          (void *)&context->mutex_h,          &mutex_c,           (HyModuleCreateHandleCb_t)HyThreadMutexCreate,    (HyModuleDestroyHandleCb_t)HyThreadMutexDestroy},
        {"thread_pool",     (void *)&context->thread_pool_h,    &thread_pool_c,     (HyModuleCreateHandleCb_t)HyThreadPoolCreate,     (HyModuleDestroyHandleCb_t)HyThreadPoolDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

static void _task_cb(void* args, void *run_befor_cb_args)
{
    _main_context_s *context = args;

    HyThreadMutexLock(context->mutex_h);
    context->num++;
    LOGI("num: %d \n", context->num);
    HyThreadMutexUnLock(context->mutex_h);
}

int main(int argc, char *argv[])
{
    _main_context_s *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_s *, sizeof(*context));

        struct {
            const char *name;
            hy_s32_t (*create)(_main_context_s *context);
        } create_arr[] = {
            {"_bool_module_create",     _bool_module_create},
            {"_handle_module_create",   _handle_module_create},
        };
        for (size_t i = 0; i < HY_UTILS_ARRAY_CNT(create_arr); i++) {
            if (create_arr[i].create) {
                if (0 != create_arr[i].create(context)) {
                    LOGE("%s failed \n", create_arr[i].name);
                }
            }
        }

        LOGE("version: %s, data: %s, time: %s \n", VERSION, __DATE__, __TIME__);

        HyThreadPoolTask_s task;
        task.task_cb    = _task_cb;
        task.args       = context;
        for (int i = 0; i < _TEST_TASK_NUM; ++i) {
            HyThreadPoolAddTask(context->thread_pool_h, &task);
        }

        while (!context->is_exit) {
            sleep(1);
        }
    } while (0);

    void (*destroy_arr[])(_main_context_s **context_pp) = {
        _handle_module_destroy,
        _bool_module_destroy
    };
    for (size_t i = 0; i < HY_UTILS_ARRAY_CNT(destroy_arr); i++) {
        if (destroy_arr[i]) {
            destroy_arr[i](&context);
        }
    }
    HY_MEM_FREE_PP(&context);

    return 0;
}
