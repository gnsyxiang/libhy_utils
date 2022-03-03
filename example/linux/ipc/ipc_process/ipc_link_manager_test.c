/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link_manager_test.c
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

#define _IPC_LINK_IPC_NAME "ipc_link"

typedef struct {
    void        *log_handle;
    void        *signal_handle;

    void        *ipc_link_manager_h;

    hy_s32_t    exit_flag;
} _main_context_t;

static void _ipc_link_manager_accept_cb(void *ipc_link_h, void *args)
{
    LOGD("ipc_link_h: %p \n", ipc_link_h);
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
        {"ipc link manager",    &context->ipc_link_manager_h,   ipc_link_manager_destroy},
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

    ipc_link_manager_config_s ipc_link_manager_config;
    HY_MEMSET(&ipc_link_manager_config, sizeof(ipc_link_manager_config));
    ipc_link_manager_config.save_config.accept_cb   = _ipc_link_manager_accept_cb;
    ipc_link_manager_config.save_config.args        = context;
    ipc_link_manager_config.ipc_name                = _IPC_LINK_IPC_NAME;
    ipc_link_manager_config.tag                     = "ipc_link_server";

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",                 &context->log_handle,           &log_config,                (create_t)HyLogCreate,                  HyLogDestroy},
        {"signal",              &context->signal_handle,        &signal_config,             (create_t)HySignalCreate,               HySignalDestroy},
        {"ipc link manager",    &context->ipc_link_manager_h,   &ipc_link_manager_config,   (create_t)ipc_link_manager_create,      ipc_link_manager_destroy},
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

