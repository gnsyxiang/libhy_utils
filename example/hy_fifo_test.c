/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_fifo_test.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 10:57
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 10:57
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hy_fifo.h"

#include "hy_hal/hy_module.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_type.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_thread.h"

#include "hy_log.h"

typedef struct {
    void *log_handle;
    void *fifo_handle;
    void *signal_handle;
    void *thread_handle;

    hy_s32_t exit_flag;
} _main_context_t;

static hy_s32_t _get_fifo_loop_cb(void *args)
{
    _main_context_t *context = args;

    hy_u32_t len;
    char c;
    while (!context->exit_flag) {
        len = HyFifoGetInfo(context->fifo_handle, HY_FIFO_INFO_USED_LEN);
        if (len > 0) {
            HyFifoRead(context->fifo_handle, &c, 1);
            LOGD("c: %c \n", c);
        }

        sleep(1);
    }

    return -1;
}

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_context_t *context = args;
    context->exit_flag = 1;
}

static void _signal_user_cb(void *args)
{
    LOGI("------user cb\n");

    _main_context_t *context = args;
    context->exit_flag = 1;
}

static void _module_destroy(_main_context_t **context_pp)
{
    _main_context_t *context = *context_pp;

    // note: 增加或删除要同步到module_create_t中
    module_destroy_t module[] = {
        {"thread",      &context->thread_handle,    HyThreadDestroy},
        {"fifo",        &context->fifo_handle,      HyFifoDestroy},
        {"signal",      &context->signal_handle,    HySignalDestroy},
        {"log",         &context->log_handle,       HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_t log_config;
    log_config.save_config.buf_len      = 16 * 1024;
    log_config.save_config.level        = HY_LOG_LEVEL_DEBUG;
    log_config.save_config.color_enable = HY_TYPE_FLAG_ENABLE;

    hy_s8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    hy_s8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGINT, SIGTERM, SIGUSR1, SIGUSR2,
    };

    HySignalConfig_t signal_config;
    memset(&signal_config, 0, sizeof(signal_config));
    HY_MEMCPY(signal_config.error_num, signal_error_num, sizeof(signal_error_num));
    HY_MEMCPY(signal_config.user_num, signal_user_num, sizeof(signal_user_num));
    signal_config.save_config.app_name      = "template";
    signal_config.save_config.coredump_path = "./";
    signal_config.save_config.error_cb      = _signal_error_cb;
    signal_config.save_config.user_cb       = _signal_user_cb;
    signal_config.save_config.args          = context;

    HyFifoConfig_t fifo_config;
    fifo_config.save_config.len = 15;
    fifo_config.save_config.mutex_flag = HY_FIFO_MUTEX_LOCK;

    HyThreadConfig_t thread_config;
    thread_config.save_config.thread_loop_cb    = _get_fifo_loop_cb;
    thread_config.save_config.args              = context;
    #define _THREAD_NAME "get_fifo"
    HY_STRNCPY(thread_config.save_config.name, HY_THREAD_NAME_LEN_MAX, _THREAD_NAME, HY_STRLEN(_THREAD_NAME));

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",         &context->log_handle,       &log_config,        (create_t)HyLogCreate,      HyLogDestroy},
        {"signal",      &context->signal_handle,    &signal_config,     (create_t)HySignalCreate,   HySignalDestroy},
        {"fifo",        &context->fifo_handle,      &fifo_config,       (create_t)HyFifoCreate,     HyFifoDestroy},
        {"thread",      &context->thread_handle,    &thread_config,     (create_t)HyThreadCreate,   HyThreadDestroy},
    };

    RUN_CREATE(module);

    return context;
}

int main(int argc, char *argv[])
{
    _main_context_t *context = _module_create();
    if (!context) {
        LOGE("_module_create faild \n");
        return -1;
    }

    LOGI("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    hy_u32_t cnt = 0;
    char c = 'a';
    hy_u32_t ret = 0;
    while (!context->exit_flag) {
        ret = HyFifoWrite(context->fifo_handle, &c, 1);
        while (!context->exit_flag && ret == 0) {
            usleep(500 * 1000);
            ret = HyFifoWrite(context->fifo_handle, &c, 1);
        }
        cnt += 1;

        LOGD("c: %c, cnt: %d \n", c, cnt);

        c += 1;
        if (c - 'a' >= 26) {
            c = 'a';
        }

        usleep(500 * 1000);
    }

    HyFifoDump(context->fifo_handle, HY_FIFO_DUMP_CONTENT);

    HyFifoDump(context->fifo_handle, HY_FIFO_DUMP_ALL);

    _module_destroy(&context);

    return 0;
}
