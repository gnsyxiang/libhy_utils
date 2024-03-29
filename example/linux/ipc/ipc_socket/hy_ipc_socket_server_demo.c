/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket_server_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 14:27
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 14:27
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_hal_utils.h"

#include "hy_ipc_socket.h"

#define _APP_NAME "hy_ipc_socket_server_demo"
#define _IPC_SOCKET_IPC_NAME    "hy_ipc_server"

typedef struct {
    void        *ipc_socket_h;
    void        *args;
} _accept_s;

typedef struct {
    void        *thread_handle;

    void        *ipc_socket_h;

    hy_s32_t    exit_flag;
} _main_context_t;

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

static hy_s32_t _bool_module_create(_main_context_t *context)
{
    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(log_c));
    log_c.config_file               = "../res/hy_log/zlog.conf";
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.mode               = HY_LOG_MODE_PROCESS_SINGLE;
    log_c.save_c.level              = HY_LOG_LEVEL_TRACE;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL;

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

static hy_s32_t _socket_communication(void *args)
{
    LOGT("args: %p \n", args);

    _accept_s *accept = args;
    _main_context_t *context = accept->args;
    char buf[8] = {0};
    hy_s32_t ret = 0;

    while (!context->exit_flag) {
        ret = HyIpcSocketRead(accept->ipc_socket_h, buf, sizeof(buf));
        if (ret < 0) {
            LOGE("HyIpcSocketRead failed, ipc_socket_h: %p \n",
                    accept->ipc_socket_h);
            break;
        }

        LOGI("buf: %s \n", buf);
    }

    HyIpcSocketDestroy(&accept->ipc_socket_h);
    HY_MEM_FREE_PP(&accept);

    return -1;
}

static void _accept_cb(void *handle, void *args)
{
    LOGT("handle: %p, args: %p \n", handle, args);

    _accept_s *accept = HY_MEM_MALLOC_RET(_accept_s *, sizeof(*accept));

    accept->args = args;
    accept->ipc_socket_h = handle;

    HyThreadConfig_s thread_c;
    const char *thread_name = "hy_socket_communication";
    HY_MEMSET(&thread_c, sizeof(thread_c));
    thread_c.save_c.thread_loop_cb    = _socket_communication;
    thread_c.save_c.args              = accept;
    thread_c.save_c.destroy_mode      = HY_THREAD_DESTROY_MODE_FORCE;
    thread_c.save_c.detach_mode       = HY_THREAD_DETACH_MODE_YES;
    HY_STRNCPY(thread_c.save_c.name, HY_THREAD_NAME_LEN_MAX,
            thread_name, HY_STRLEN("thread_name"));

    if (!HyThreadCreate(&thread_c)) {
        LOGE("HyThreadCreate failed \n");
    }
}

static hy_s32_t _thread_loop_cb(void *args)
{
    return HyIpcSocketAccept(((_main_context_t *)args)->ipc_socket_h,
            _accept_cb, args);
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

        LOGE("version: %s, data: %s, time: %s \n", VERSION, __DATE__, __TIME__);

        context->ipc_socket_h = HyIpcSocketCreate_m(_IPC_SOCKET_IPC_NAME,
                HY_IPC_SOCKET_TYPE_SERVER);
        if (!context->ipc_socket_h) {
            LOGE("HyIpcSocketCreate failed \n");
        }

        LOGI("fd: %d \n",       HyIpcSocketGetFD(context->ipc_socket_h));
        LOGI("type: %d \n",     HyIpcSocketGetType(context->ipc_socket_h));
        LOGI("ipc_name: %s \n", HyIpcSocketGetName(context->ipc_socket_h));

        context->thread_handle = HyThreadCreate_m("hy_accept",
                _thread_loop_cb, context);
        if (!context->thread_handle) {
            LOGE("HyThreadCreate_m failed \n");
        }

        while (!context->exit_flag) {
            sleep(1);
        }

    } while (0);

    HyIpcSocketDestroy(&context->ipc_socket_h);

    HyThreadDestroy(&context->thread_handle);

    // mem leak for waitting thread exit
    sleep(2);

    void (*destroy_arr[])(_main_context_s **context_pp) = {
        _bool_module_destroy
    };
    for (hy_u32_t i = 0; i < HY_UTILS_ARRAY_CNT(destroy_arr); i++) {
        if (destroy_arr[i]) {
            destroy_arr[i](&context);
        }
    }
    HY_MEM_FREE_PP(&context);

    return 0;
}

