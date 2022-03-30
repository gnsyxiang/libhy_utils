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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#include "hy_thread_pool.h"

#define _APP_NAME "hy_thread_pool_demo"

typedef struct {
    void        *log_h;
    void        *signal_h;

    void        *thread_pool_h;

    hy_s32_t    exit_flag;
} _main_context_t;

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_context_t *context = args;
    context->exit_flag = 1;
}

static void _signal_user_cb(void *args)
{
    LOGW("------user cb\n");

    _main_context_t *context = args;
    context->exit_flag = 1;
}

static void _module_destroy(_main_context_t **context_pp)
{
    _main_context_t *context = *context_pp;

    // note: 增加或删除要同步到module_create_t中
    module_destroy_t module[] = {
        {"thread pool", &context->thread_pool_h,        HyThreadPoolDestroy},
        {"signal",      &context->signal_h,             HySignalDestroy},
        {"log",         &context->log_h,                HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_s log_c;
    log_c.save_c.buf_len_min  = 512;
    log_c.save_c.buf_len_max  = 512;
    log_c.save_c.level        = HY_LOG_LEVEL_DEBUG;
    log_c.save_c.color_enable = HY_TYPE_FLAG_ENABLE;

    int8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    int8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGINT, SIGTERM, SIGUSR1, SIGUSR2,
    };

    HySignalConfig_t signal_c;
    memset(&signal_c, 0, sizeof(signal_c));
    HY_MEMCPY(signal_c.error_num, signal_error_num, sizeof(signal_error_num));
    HY_MEMCPY(signal_c.user_num, signal_user_num, sizeof(signal_user_num));
    signal_c.save_c.app_name      = _APP_NAME;
    signal_c.save_c.coredump_path = "./";
    signal_c.save_c.error_cb      = _signal_error_cb;
    signal_c.save_c.user_cb       = _signal_user_cb;
    signal_c.save_c.args          = context;

    HyThreadPoolConfig_s thread_pool_config;
    thread_pool_config.shutdown_flag    = HY_THREAD_POOL_DESTROY_GRACEFUL;
    thread_pool_config.thread_max_cnt   = 4;
    thread_pool_config.task_max_cnt     = 12;

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",         &context->log_h,                &log_c,                 (create_t)HyLogCreate,          HyLogDestroy},
        {"signal",      &context->signal_h,             &signal_c,              (create_t)HySignalCreate,       HySignalDestroy},
        {"thread pool", &context->thread_pool_h,        &thread_pool_config,    (create_t)HyThreadPoolCreate,   HyThreadPoolDestroy},
    };

    RUN_CREATE(module);

    return context;
}

static void _task_cb(void *args)
{
    usleep(500 * 1000);
    LOGI("----haha\n");
}

int main(int argc, char *argv[])
{
    _main_context_t *context = _module_create();
    if (!context) {
        LOGE("_module_create faild \n");
        return -1;
    }

    LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    hy_s32_t ret;
    for (hy_s32_t i = 0; i < 20; ++i) {
        ret = HyThreadPoolAdd(context->thread_pool_h, _task_cb, context);
        if (0 != ret) {
            LOGE("thread pool add failed \n");
        }
    }

    while (!context->exit_flag) {
        sleep(1);
    }

    _module_destroy(&context);

    return 0;
}
