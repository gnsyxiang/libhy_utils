/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process_client_set_test.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    07/03 2022 11:58
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        07/03 2022      create the file
 * 
 *     last modified: 07/03 2022 11:58
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

#define _IPC_PROCESS_IPC_NAME "ipc_process"

typedef struct {
    void        *log_handle;
    void        *signal_handle;

    void        *ipc_process_client_h;

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

    // note: 增加或删除要同步到module_create_t中
    module_destroy_t module[] = {
        {"ipc process client",  &context->ipc_process_client_h,     HyIpcProcessDestroy},
        {"signal",              &context->signal_handle,            HySignalDestroy},
        {"log",                 &context->log_handle,               HyLogDestroy},
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
    log_config.save_config.level        = HY_LOG_LEVEL_DEBUG;
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

    HyIpcProcessFunc_s func[] = {
    };

    HyIpcProcessConfig_s ipc_process_c;
    HY_MEMSET(&ipc_process_c, sizeof(ipc_process_c));
    ipc_process_c.save_config.state_change_cb   = _state_change_cb;
    ipc_process_c.save_config.args              = context;
    ipc_process_c.save_config.type              = HY_IPC_PROCESS_TYPE_CLIENT;
    ipc_process_c.ipc_name                      = _IPC_PROCESS_IPC_NAME;
    ipc_process_c.tag                           = "ipc_process_set_client";
    ipc_process_c.func                          = func;
    ipc_process_c.func_args                     = context;
    ipc_process_c.func_cnt                      = HY_UTILS_ARRAY_CNT(func);

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",                     &context->log_handle,               &log_config,        (create_t)HyLogCreate,          HyLogDestroy},
        {"signal",                  &context->signal_handle,            &signal_config,     (create_t)HySignalCreate,       HySignalDestroy},
        {"ipc process client",      &context->ipc_process_client_h,     &ipc_process_c,     (create_t)HyIpcProcessCreate,   HyIpcProcessDestroy},
    };

    RUN_CREATE(module);

    return context;
}

static void _audio_param_get(_main_context_t *context)
{
    hy_s32_t ret = -1;
    HyIpcProcessAudioParamGet_s audio_param_get;
    HyIpcProcessAudioParamGetResult_s audio_param_get_ret;

    HY_MEMSET(&audio_param_get, sizeof(audio_param_get));
    HY_MEMSET(&audio_param_get_ret, sizeof(audio_param_get_ret));

    audio_param_get.type = 0x01;

    ret = HyIpcProcessDataSync(context->ipc_process_client_h,
            HY_IPC_PROCESS_MSG_ID_SYNC_DATA_AUDIO_PARAM_GET,
            &audio_param_get, sizeof(audio_param_get),
            &audio_param_get_ret, sizeof(audio_param_get_ret));
    if (0 != ret) {
        LOGE("HyIpcProcessDataSync failed, id: %d \n", HY_IPC_PROCESS_MSG_ID_SYNC_DATA_AUDIO_PARAM_GET);
        return ;
    }

    LOGD("channel: %d \n",          audio_param_get_ret.channel);
    LOGD("sample_rate: %d \n",      audio_param_get_ret.sample_rate);
    LOGD("bit_per_sample: %d \n",   audio_param_get_ret.bit_per_sample);
}

static void _audio_param_set(_main_context_t *context)
{
    hy_s32_t ret = -1;
    HyIpcProcessAudioParamSet_s audio_param_set;
    HyIpcProcessAudioParamSetResult_s audio_param_set_ret;

    HY_MEMSET(&audio_param_set, sizeof(audio_param_set));
    HY_MEMSET(&audio_param_set_ret, sizeof(audio_param_set_ret));

    audio_param_set.channel         = 2;
    audio_param_set.sample_rate     = 16 * 1000;
    audio_param_set.bit_per_sample  = 16;

    ret = HyIpcProcessDataSync(context->ipc_process_client_h,
            HY_IPC_PROCESS_MSG_ID_SYNC_DATA_AUDIO_PARAM_SET,
            &audio_param_set, sizeof(audio_param_set),
            &audio_param_set_ret, sizeof(audio_param_set_ret));
    if (0 != ret) {
        LOGE("HyIpcProcessDataSync failed, id: %d \n", HY_IPC_PROCESS_MSG_ID_SYNC_DATA_AUDIO_PARAM_SET);
        return ;
    }
}

int main(int argc, char *argv[])
{
    _main_context_t *context = _module_create();
    if (!context) {
        LOGE("_module_create faild \n");
        return -1;
    }

    LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    sleep(1);
    _audio_param_get(context);
    _audio_param_set(context);
    _audio_param_get(context);

    while (!context->exit_flag) {
        sleep(1);
    }

    _module_destroy(&context);

    return 0;
}

