/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket_process_server_test.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/02 2022 11:12
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/02 2022      create the file
 * 
 *     last modified: 17/02 2022 11:12
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

#include "hy_utils.h"

#include "hy_ipc_socket_process.h"

#define IPC_SOCKET_NAME "ipc_socket"

typedef struct {
    void        *log_handle;
    void        *signal_handle;
    void        *ipc_process_handle;

    hy_s32_t    exit_flag;
} _main_context_t;

static void _ipc_process_connect_change_cb(void *handle,
        HyIpcSocketProcessConnectState_e is_connect, void *args)
{
    LOGD("%s \n", is_connect == HY_IPC_SOCKET_PROCESS_STATE_CONNECT
            ? "connect" : "disconnect");
}

static hy_s32_t _ipc_process_test_cb(void *server_handle,
        void *client_handle, void *args)
{
    LOGD("test cb \n");

    return 0;
}

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
        {"ipc process handle",  &context->ipc_process_handle,   HyIpcSocketProcessDestroy},
        {"signal",              &context->signal_handle,        HySignalDestroy},
        {"log",                 &context->log_handle,           HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_s log_config;
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

    HyIpcSocketProcessCallbackCb_s ipc_process_cb[] = {
        {HY_IPC_SOCKET_PROCESS_ID_TEST, _ipc_process_test_cb},
    };

    HyIpcSocketProcessConfig_s ipc_process_config;
    HY_MEMSET(&ipc_process_config, sizeof(ipc_process_config));
    ipc_process_config.save_config.connect_change   = _ipc_process_connect_change_cb;
    ipc_process_config.save_config.args             = context;
    ipc_process_config.save_config.type             = HY_IPC_SOCKET_PROCESS_TYPE_SERVER;
    ipc_process_config.ipc_name                     = IPC_SOCKET_NAME;
    ipc_process_config.tag                          = "server";
    ipc_process_config.callback                     = ipc_process_cb;
    ipc_process_config.callback_cnt                 = HY_UTILS_ARRAY_CNT(ipc_process_cb);

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",                 &context->log_handle,           &log_config,            (create_t)HyLogCreate,                  HyLogDestroy},
        {"signal",              &context->signal_handle,        &signal_config,         (create_t)HySignalCreate,               HySignalDestroy},
        {"ipc process handle",  &context->ipc_process_handle,   &ipc_process_config,    (create_t)HyIpcSocketProcessCreate,     HyIpcSocketProcessDestroy},
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

    LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    while (!context->exit_flag) {
        sleep(1);
    }

    _module_destroy(&context);

    return 0;
}
