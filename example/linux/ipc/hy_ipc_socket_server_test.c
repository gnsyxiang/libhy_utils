/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket_server_test.c
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

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#include "hy_ipc_socket.h"

#define _IPC_SOCKET_IPC_NAME    "hy_ipc_server"

typedef struct {
    void *log_handle;
    void *signal_handle;

    void *ipc_socket_handle;

    hy_s32_t exit_flag;
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
        {"socket server",   &context->ipc_socket_handle,    HyIpcSocketDestroy},
        {"signal",          &context->signal_handle,        HySignalDestroy},
        {"log",             &context->log_handle,           HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_t log_config;
    log_config.save_config.buf_len_min  = 512;
    log_config.save_config.buf_len_max  = 512;
    log_config.save_config.level        = HY_LOG_LEVEL_TRACE;
    log_config.save_config.color_enable = HY_TYPE_FLAG_ENABLE;

    int8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    int8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGINT, SIGTERM, SIGUSR1, SIGUSR2,
    };

    HySignalConfig_t signal_config;
    HY_MEMSET(&signal_config, sizeof(signal_config));
    HY_MEMCPY(signal_config.error_num, signal_error_num, sizeof(signal_error_num));
    HY_MEMCPY(signal_config.user_num, signal_user_num, sizeof(signal_user_num));
    signal_config.save_config.app_name      = "template";
    signal_config.save_config.coredump_path = "./";
    signal_config.save_config.error_cb      = _signal_error_cb;
    signal_config.save_config.user_cb       = _signal_user_cb;
    signal_config.save_config.args          = context;

    HyIpcSocketConfig_s ipc_socket_config;
    HY_MEMSET(&ipc_socket_config, sizeof(ipc_socket_config));
    HY_MEMCPY(ipc_socket_config.save_config.ipc_name, _IPC_SOCKET_IPC_NAME, HY_STRLEN(_IPC_SOCKET_IPC_NAME));
    ipc_socket_config.save_config.type = HY_IPC_SOCKET_TYPE_SERVER;

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",             &context->log_handle,           &log_config,            (create_t)HyLogCreate,          HyLogDestroy},
        {"signal",          &context->signal_handle,        &signal_config,         (create_t)HySignalCreate,       HySignalDestroy},
        {"socket server",   &context->ipc_socket_handle,    &ipc_socket_config,     (create_t)HyIpcSocketCreate,    HyIpcSocketDestroy},
    };

    RUN_CREATE(module);

    return context;
}

// static hy_s32_t _socket_communication(void *args)
// {
    // _main_context_t *context = args;
//
    // char buf[4] = {0};
    // hy_s32_t ret = 0;
//
    // while (!context->exit_flag) {
        // ret = HyIpcSocketRead(context->ipc_socket_handle, buf, 2);
        // if (ret < 0) {
            // LOGE("HyIpcSocketRead failed \n");
            // break;
        // }
    // }
//
    // return -1;
// }

static void _accept_cb(hy_s32_t fd,
        const char *ipc_name, void *args)
{
    LOGD("fd: %d, ipc_name: %s, \n", fd, ipc_name);
    _main_context_t *context = args;

    char buf[8] = {0};
    hy_s32_t ret = 0;

    while (!context->exit_flag) {
        HY_MEMSET(buf, sizeof(buf));

        ret = read(fd, buf, sizeof(buf));
        if (ret < 0) {
            LOGE("HyIpcSocketRead failed \n");
            break;
        }

        LOGE("buf: %s \n", buf);
    }

    // HyThreadConfig_t thread_config;
    // HY_MEMSET(&thread_config, sizeof(thread_config));
    // thread_config.save_config.thread_loop_cb    = _socket_communication;
    // thread_config.save_config.args              = args;
    // thread_config.save_config.destroy_flag      = HY_THREAD_DESTROY_FORCE;
    // thread_config.save_config.detach_flag       = HY_THREAD_DETACH_YES;
    // HY_STRNCPY(thread_config.save_config.name,
            // HY_THREAD_NAME_LEN_MAX, "hy_socket_communication", HY_STRLEN("hy_socket_communication"));
//
    // if (!HyThreadCreate(&thread_config)) {
        // LOGE("HyThreadCreate failed \n");
    // }
}

int main(int argc, char *argv[])
{
    _main_context_t *context = _module_create();
    if (!context) {
        LOGE("_module_create faild \n");
        return -1;
    }

    LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    HyIpcSocketAccept(context->ipc_socket_handle, _accept_cb, context);

    _module_destroy(&context);

    return 0;
}