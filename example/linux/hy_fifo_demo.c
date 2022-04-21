/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_fifo_demo.c
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

#include "hy_hal/hy_module.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_type.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_log.h"

#include "hy_fifo.h"

#define _APP_NAME "hy_fifo_demo"

typedef struct {
    void        *log_h;
    void        *signal_h;
    void        *fifo_h;
    void        *thread_h;

    hy_s32_t    exit_flag;
} _main_context_t;

static hy_s32_t _get_fifo_loop_cb(void *args)
{
    _main_context_t *context = args;

    hy_u32_t len;
    char c;
    while (!context->exit_flag) {
        len = HyFifoGetInfo(context->fifo_h, HY_FIFO_INFO_USED_LEN);
        if (len > 0) {
            HyFifoRead(context->fifo_h, &c, 1);
            LOGI("--read---------, c: %c \n", c);
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
        {"thread",      &context->thread_h,     HyThreadDestroy},
        {"fifo",        &context->fifo_h,       HyFifoDestroy},
        {"signal",      &context->signal_h,     HySignalDestroy},
        {"log",         &context->log_h,        HyLogDestroy},
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
    log_c.save_c.level        = HY_LOG_LEVEL_TRACE;
    log_c.save_c.color_enable = HY_TYPE_FLAG_ENABLE;

    hy_s8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    hy_s8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
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

    HyFifoConfig_s fifo_c;
    fifo_c.save_c.len           = 25;
    fifo_c.save_c.is_lock       = HY_FIFO_MUTEX_LOCK;

    HyThreadConfig_s thread_c;
    HY_MEMSET(&thread_c, sizeof(thread_c));
    thread_c.save_c.thread_loop_cb    = _get_fifo_loop_cb;
    thread_c.save_c.args              = context;
    #define _THREAD_NAME "get_fifo"
    HY_STRNCPY(thread_c.save_c.name, HY_THREAD_NAME_LEN_MAX,
            _THREAD_NAME, HY_STRLEN(_THREAD_NAME));

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",         &context->log_h,        &log_c,             (create_t)HyLogCreate,      HyLogDestroy},
        {"signal",      &context->signal_h,     &signal_c,          (create_t)HySignalCreate,   HySignalDestroy},
        {"fifo",        &context->fifo_h,       &fifo_c,            (create_t)HyFifoCreate,     HyFifoDestroy},
        {"thread",      &context->thread_h,     &thread_c,          (create_t)HyThreadCreate,   HyThreadDestroy},
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
        ret = HyFifoWrite(context->fifo_h, &c, 1);
        while (!context->exit_flag && ret == 0) {
            usleep(500 * 1000);
            ret = HyFifoWrite(context->fifo_h, &c, 1);
        }
        cnt += 1;

        LOGD("--write--, c: %c, cnt: %d \n", c, cnt);

        c += 1;
        if (c - 'a' >= 26) {
            c = 'a';
        }

        usleep(500 * 1000);
    }

    HyFifoDump(context->fifo_h, HY_FIFO_DUMP_CONTENT);

    HyFifoDump(context->fifo_h, HY_FIFO_DUMP_ALL);

    _module_destroy(&context);

    return 0;
}

