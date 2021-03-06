/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process_client_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 17:05
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 17:05
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#include "hy_ipc_process.h"
#include "hy_utils.h"

#define _APP_NAME "hy_ipc_process_client_demo"
#define _IPC_PROCESS_IPC_NAME "ipc_process"

typedef struct {
    void        *ipc_process_client_h;

    hy_u32_t    channel;
    hy_u32_t    sample_rate;
    hy_u32_t    bit_per_sample;

    hy_u32_t    width;
    hy_u32_t    height;

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

static hy_s32_t _audio_param_get_cb(void *recv, hy_u32_t recv_len,
        void *send, hy_u32_t send_len, void *args)
{
    HyIpcProcessAudioParamGet_s *audio_param_get = NULL;
    HyIpcProcessAudioParamGetResult_s *audio_param_get_ret = NULL;
    _main_context_t *context = args;

    audio_param_get = (HyIpcProcessAudioParamGet_s *)recv;
    audio_param_get_ret = (HyIpcProcessAudioParamGetResult_s *)send;

    LOGD("audio_param_get.type: %d \n", audio_param_get->type);

    audio_param_get_ret->channel        = context->channel;
    audio_param_get_ret->sample_rate    = context->sample_rate;
    audio_param_get_ret->bit_per_sample = context->bit_per_sample;

    LOGI("audio param get ok \n");
    return 0;
}

static hy_s32_t _audio_param_set_cb(void *recv, hy_u32_t recv_len,
        void *send, hy_u32_t send_len, void *args)
{
    HyIpcProcessAudioParamSet_s *audio_param_set = recv;
    _main_context_t *context = args;

    LOGD("channel: %d \n",          audio_param_set->channel);
    LOGD("sample_rate: %d \n",      audio_param_set->sample_rate);
    LOGD("bit_per_sample: %d \n",   audio_param_set->bit_per_sample);

    context->channel        = audio_param_set->channel;
    context->sample_rate    = audio_param_set->sample_rate;
    context->bit_per_sample = audio_param_set->bit_per_sample;

    sleep(1);

    LOGI("audio param set ok \n");
    return 0;
}

static hy_s32_t _video_param_get_cb(void *recv, hy_u32_t recv_len,
        void *send, hy_u32_t send_len, void *args)
{
    HyIpcProcessVideoParamGetResult_s *video_param_get_ret = NULL;
    _main_context_t *context = args;

    video_param_get_ret = (HyIpcProcessVideoParamGetResult_s *)send;

    video_param_get_ret->width = context->width;
    video_param_get_ret->height = context->height;

    LOGI("video param get ok \n");
    return 0;
}

static hy_s32_t _video_param_set_cb(void *recv, hy_u32_t recv_len,
        void *send, hy_u32_t send_len, void *args)
{
    HyIpcProcessVideoParamSet_s *video_param_set = recv;
    _main_context_t *context = args;

    context->width = video_param_set->width;
    context->height = video_param_set->height;

    LOGI("video param set ok \n");
    return 0;
}

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

        LOGD("connect to server \n");
    } else {
        context->exit_flag = 1;
        LOGD("disconnect to server \n");
    }
}

static void _handle_module_destroy(_main_context_t *context)
{
    // note: ???????????????????????????HyModuleCreateHandle_s???
    HyModuleDestroyHandle_s module[] = {
        {"ipc process client",  &context->ipc_process_client_h,     HyIpcProcessDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_t *context)
{
    HyIpcProcessFunc_s func[] = {
        {HY_IPC_PROCESS_MSG_ID_SYNC_AUDIO_PARAM_GET,   _audio_param_get_cb},
        {HY_IPC_PROCESS_MSG_ID_SYNC_AUDIO_PARAM_SET,   _audio_param_set_cb},

        {HY_IPC_PROCESS_MSG_ID_SYNC_VIDEO_PARAM_GET,   _video_param_get_cb},
        {HY_IPC_PROCESS_MSG_ID_SYNC_VIDEO_PARAM_SET,   _video_param_set_cb},
    };

    HyIpcProcessConfig_s ipc_process_c;
    HY_MEMSET(&ipc_process_c, sizeof(ipc_process_c));
    ipc_process_c.save_config.state_change_cb   = _state_change_cb;
    ipc_process_c.save_config.args              = context;
    ipc_process_c.save_config.type              = HY_IPC_PROCESS_TYPE_CLIENT;
    ipc_process_c.ipc_name                      = _IPC_PROCESS_IPC_NAME;
    ipc_process_c.tag                           = "ipc_process_client";
    ipc_process_c.func                          = func;
    ipc_process_c.func_args                     = context;
    ipc_process_c.func_cnt                      = HY_UTILS_ARRAY_CNT(func);
    ipc_process_c.connect_timeout_s             = 5;

    // note: ???????????????????????????HyModuleDestroyHandle_s???
    HyModuleCreateHandle_s module[] = {
        {"ipc process client",  &context->ipc_process_client_h,     &ipc_process_c,     (HyModuleCreateHandleCb_t)HyIpcProcessCreate,   HyIpcProcessDestroy},
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

        context->channel        = 1;
        context->sample_rate    = 8000;
        context->bit_per_sample = 8;

        context->width = 1280;
        context->height = 720;

        LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

        while (!context->exit_flag) {
            sleep(1);
        }
    } while (0);

    _handle_module_destroy(context);
    _bool_module_destroy();
    HY_MEM_FREE_PP(&context);

    return 0;
}

