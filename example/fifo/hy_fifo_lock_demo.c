/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_fifo_lock_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    15/05 2023 20:39
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        15/05 2023      create the file
 * 
 *     last modified: 15/05 2023 20:39
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "hy_module.h"
#include "hy_mem.h"
#include "hy_signal.h"
#include "hy_string.h"
#include "hy_type.h"
#include "hy_utils.h"
#include "hy_thread.h"

#include "hy_fifo_lock.h"

#define _APP_NAME "hy_fifo_lock_demo"

typedef struct {
    HyFifoLock_s    *fifo_h;
    HyThread_s      *read_fifo_thread_h;
    HyThread_s      *write_fifo_thread_h;

    hy_s32_t        is_exit;
} _main_context_t;

static hy_s32_t _write_fifo_loop_cb(void *args)
{
    _main_context_t *context = args;
    hy_u32_t cnt = 0;
    char c = 'a';
    hy_u32_t ret = 0;

    while (!context->is_exit) {
        ret = HyFifoLockWrite(context->fifo_h, &c, 1);
        if (ret > 0) {
            cnt += 1;

            LOGD("--write--, c: %c, cnt: %d \n", c, cnt);

            c += 1;
            if (c - 'a' >= 26) {
                c = 'a';
            }
        }

        usleep(20 * 1000);
    }

    return -1;
}

static hy_s32_t _read_fifo_loop_cb(void *args)
{
    _main_context_t *context = args;

    hy_u32_t len;
    char c;
    while (!context->is_exit) {
        len = HyFifoLockGetUsedLen(context->fifo_h);
        if (len > 0) {
            HyFifoLockRead(context->fifo_h, &c, 1);
            LOGI("--read---------, c: %c \n", c);
        }

        usleep(30 * 1000);
    }

    return -1;
}

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_context_t *context = args;
    context->is_exit = 1;
}

static void _signal_user_cb(void *args)
{
    LOGI("------user cb\n");

    _main_context_t *context = args;
    context->is_exit = 1;
}

static void _bool_module_destroy(void)
{
    HyModuleDestroyBool_s bool_module[] = {
        {"signal",          HySignalDestroy },
        {"log",             HyLogDeInit     },
    };

    HY_MODULE_RUN_DESTROY_BOOL(bool_module);
}

static hy_s32_t _bool_module_create(_main_context_t *context)
{
    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(log_c));
    log_c.config_file               = "../res/hy_log/zlog.conf";
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.level              = HY_LOG_LEVEL_TRACE;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL;

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

static void _handle_module_destroy(_main_context_t *context)
{
    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"write_fifo_thread",   (void **)&context->write_fifo_thread_h,     (HyModuleDestroyHandleCb_t)HyThreadDestroy},
        {"read_fifo_thread",    (void **)&context->read_fifo_thread_h,      (HyModuleDestroyHandleCb_t)HyThreadDestroy},
        {"fifo",                (void **)&context->fifo_h,                  (HyModuleDestroyHandleCb_t)HyFifoLockDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_t *context)
{
    HyFifoLockConfig_s fifo_c;
    HY_MEMSET(&fifo_c, sizeof(fifo_c));
    fifo_c.save_c.len           = 25;

    HyThreadConfig_s read_fifo_thread_c;
    const char *read_thread_name = "read_fifo_thread";
    HY_MEMSET(&read_fifo_thread_c, sizeof(read_fifo_thread_c));
    read_fifo_thread_c.save_c.thread_loop_cb = _read_fifo_loop_cb;
    read_fifo_thread_c.save_c.args = context;
    HY_STRNCPY(read_fifo_thread_c.save_c.name, HY_THREAD_NAME_LEN_MAX,
            read_thread_name, HY_STRLEN(read_thread_name));

    HyThreadConfig_s write_fifo_thread_c;
    const char *write_thread_name = "read_fifo_thread";
    HY_MEMSET(&write_fifo_thread_c, sizeof(write_fifo_thread_c));
    write_fifo_thread_c.save_c.thread_loop_cb = _write_fifo_loop_cb;
    write_fifo_thread_c.save_c.args = context;
    HY_STRNCPY(write_fifo_thread_c.save_c.name, HY_THREAD_NAME_LEN_MAX,
               write_thread_name, HY_STRLEN(write_thread_name));

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"fifo",                    (void **)&context->fifo_h,                  &fifo_c,                    (HyModuleCreateHandleCb_t)HyFifoLockCreate,     (HyModuleDestroyHandleCb_t)HyFifoLockDestroy},
        {"read_fifo_thread",        (void **)&context->read_fifo_thread_h,      &read_fifo_thread_c,        (HyModuleCreateHandleCb_t)HyThreadCreate,   (HyModuleDestroyHandleCb_t)HyThreadDestroy},
        {"write_fifo_thread",       (void **)&context->write_fifo_thread_h,     &write_fifo_thread_c,       (HyModuleCreateHandleCb_t)HyThreadCreate,   (HyModuleDestroyHandleCb_t)HyThreadDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

int main(int argc, char *argv[])
{
    _main_context_t *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_t *, sizeof(*context));

        if (0 != _bool_module_create(context)) {
            printf("_bool_module_create failed \n");
            break;
        }

        if (0 != _handle_module_create(context)) {
            LOGE("_handle_module_create failed \n");
            break;
        }

        LOGI("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

        while (!context->is_exit) {
            sleep(1);

            LOGI("free len: %d, used len: %d \n",
                 HyFifoLockGetFreeLen(context->fifo_h), HyFifoLockGetUsedLen(context->fifo_h));
        }
    } while (0);

    HyFifoLockDumpContent(context->fifo_h);

    HyFifoLockDumpAll(context->fifo_h);

    _handle_module_destroy(context);
    _bool_module_destroy();
    HY_MEM_FREE_PP(&context);

    return 0;
}
