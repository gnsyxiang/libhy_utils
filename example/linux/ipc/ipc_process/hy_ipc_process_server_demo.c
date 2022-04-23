/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process_server_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 17:21
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 17:21
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#include "hy_ipc_process.h"
#include "hy_utils.h"

#define _APP_NAME "hy_ipc_process_server_demo"
#define _IPC_PROCESS_IPC_NAME "ipc_process"

typedef struct {
    void        *signal_h;

    void        *ipc_process_server_h;

    hy_s32_t    exit_flag;
} _main_context_t;

static void _state_change_cb(HyIpcProcessInfo_s *ipc_process_info,
        HyIpcProcessConnectState_e is_connect, void *args)
{
    LOGT("ipc_process_info: %p, is_connect: %d, args: %p \n",
            ipc_process_info, is_connect, args);
    HY_ASSERT_RET(!args);

    _main_context_t *context = args;

    if (is_connect == HY_IPC_PROCESS_CONNECT_STATE_CONNECT) {
        LOGD("ipc_name: %s, tag: %s, pid: %d \n", ipc_process_info->ipc_name,
                ipc_process_info->tag, ipc_process_info->pid);

        LOGD("client connect server \n");
    } else {
        context->exit_flag = 1;
        LOGD("server exit \n");
    }
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

    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"ipc process server",  &context->ipc_process_server_h,     HyIpcProcessDestroy},
        {"signal",              &context->signal_h,                 HySignalDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(log_c));
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.mode               = HY_LOG_MODE_PROCESS_SINGLE;
    log_c.save_c.level              = HY_LOG_LEVEL_TRACE;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL;

    HyModuleCreateBool_s bool_module[] = {
        {"log",     &log_c,     (HyModuleCreateBoolCb_t)HyLogInit,  HyLogDeInit},
    };

    HY_MODULE_RUN_CREATE_BOOL(bool_module);

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
    signal_c.save_c.app_name      = _APP_NAME;
    signal_c.save_c.coredump_path = "./";
    signal_c.save_c.error_cb      = _signal_error_cb;
    signal_c.save_c.user_cb       = _signal_user_cb;
    signal_c.save_c.args          = context;

    HyIpcProcessFunc_s func[] = {
    };

    HyIpcProcessConfig_s ipc_process_c;
    HY_MEMSET(&ipc_process_c, sizeof(ipc_process_c));
    ipc_process_c.save_config.state_change_cb   = _state_change_cb;
    ipc_process_c.save_config.args              = context;
    ipc_process_c.save_config.type              = HY_IPC_PROCESS_TYPE_SERVER;
    ipc_process_c.ipc_name                      = _IPC_PROCESS_IPC_NAME;
    ipc_process_c.tag                           = "ipc_process_server";
    ipc_process_c.func                          = func;
    ipc_process_c.func_args                     = context;
    ipc_process_c.func_cnt                      = HY_UTILS_ARRAY_CNT(func);

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"signal",                  &context->signal_h,                 &signal_c,          (HyModuleCreateHandleCb_t)HySignalCreate,       HySignalDestroy},
        {"ipc process server",      &context->ipc_process_server_h,     &ipc_process_c,     (HyModuleCreateHandleCb_t)HyIpcProcessCreate,   HyIpcProcessDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);

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

