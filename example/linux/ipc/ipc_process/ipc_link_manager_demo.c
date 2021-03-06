/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link_manager_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 14:39
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 14:39
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

#include "ipc_link_manager.h"
#include "ipc_link.h"

#define _APP_NAME "ipc_link_manager_demo"
#define _IPC_LINK_IPC_NAME "ipc_link"

typedef struct {
    void        *ipc_link_h;
    void        *ipc_link_manager_h;

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
    context->ipc_link_h = ipc_link_create_m(_IPC_LINK_IPC_NAME,
            "ipc_link_server", IPC_LINK_TYPE_SERVER, NULL);
    if (!context->ipc_link_h) {
        LOGE("ipc link create m failed \n");
        return -1;
    }

    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(log_c));
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.mode               = HY_LOG_MODE_PROCESS_SINGLE;
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

static void _ipc_link_manager_accept_cb(void *ipc_link_h, void *args)
{
    LOGD("ipc_link_h: %p \n", ipc_link_h);
}

static void _handle_module_destroy(_main_context_t *context)
{
    // note: ???????????????????????????HyModuleCreateHandle_s???
    HyModuleDestroyHandle_s module[] = {
        {"ipc link manager",    &context->ipc_link_manager_h,   ipc_link_manager_destroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_t *context)
{
    ipc_link_manager_config_s ipc_link_manager_c;
    HY_MEMSET(&ipc_link_manager_c, sizeof(ipc_link_manager_c));
    ipc_link_manager_c.save_config.accept_cb    = _ipc_link_manager_accept_cb;
    ipc_link_manager_c.save_config.args         = context;
    ipc_link_manager_c.ipc_link_h               = context->ipc_link_h;

    // note: ???????????????????????????HyModuleDestroyHandle_s???
    HyModuleCreateHandle_s module[] = {
        {"ipc link manager",    &context->ipc_link_manager_h,   &ipc_link_manager_c,        (HyModuleCreateHandleCb_t)ipc_link_manager_create,      ipc_link_manager_destroy},
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

        LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

        while (!context->exit_flag) {
            sleep(1);
        }

    } while (0);

    ipc_link_destroy(&context->ipc_link_h);

    _handle_module_destroy(context);
    _bool_module_destroy();
    HY_MEM_FREE_PP(&context);

    return 0;
}

