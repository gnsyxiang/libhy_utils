/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process_client_set_demo.c
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
#include "hy_hal/hy_thread.h"

#include "hy_ipc_process.h"
#include "hy_utils.h"

#define _APP_NAME "hy_ipc_process_client_set_demo"
#define _IPC_PROCESS_IPC_NAME "ipc_process"

typedef struct {
    void        *signal_h;

    void        *ipc_process_client_h;
    void        *audio_thread_h;
    void        *video_thread_h;

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
        {"ipc process client",  &context->ipc_process_client_h,     HyIpcProcessDestroy},
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
    ipc_process_c.save_config.type              = HY_IPC_PROCESS_TYPE_CLIENT;
    ipc_process_c.ipc_name                      = _IPC_PROCESS_IPC_NAME;
    ipc_process_c.tag                           = "ipc_process_set_client";
    ipc_process_c.func                          = func;
    ipc_process_c.func_args                     = context;
    ipc_process_c.func_cnt                      = HY_UTILS_ARRAY_CNT(func);

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"signal",                  &context->signal_h,                 &signal_c,          (HyModuleCreateHandleCb_t)HySignalCreate,       HySignalDestroy},
        {"ipc process client",      &context->ipc_process_client_h,     &ipc_process_c,     (HyModuleCreateHandleCb_t)HyIpcProcessCreate,   HyIpcProcessDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);

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

    ret = HyIpcProcessDataSync_m(context->ipc_process_client_h,
            HY_IPC_PROCESS_MSG_ID_SYNC_AUDIO_PARAM_GET,
            &audio_param_get, &audio_param_get_ret, 3);
    if (0 != ret) {
        LOGE("HyIpcProcessDataSync_m failed, id: %d \n", ret);
        return ;
    }

    LOGD("channel: %d \n",          audio_param_get_ret.channel);
    LOGD("sample_rate: %d \n",      audio_param_get_ret.sample_rate);
    LOGD("bit_per_sample: %d \n",   audio_param_get_ret.bit_per_sample);

    LOGD("get audio \n");
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

    ret = HyIpcProcessDataSync_m(context->ipc_process_client_h,
            HY_IPC_PROCESS_MSG_ID_SYNC_AUDIO_PARAM_SET,
            &audio_param_set, &audio_param_set_ret, 3);
    if (0 != ret) {
        LOGE("HyIpcProcessDataSync_m failed, id: %d \n", ret);
        return ;
    }

    LOGD("set audio \n");
}

static hy_s32_t _ipcst_audio_thread_cb(void *args)
{
    _main_context_t *context = args;

    sleep(1);
    _audio_param_get(context);
    _audio_param_set(context);
    _audio_param_get(context);

    return -1;
}

static void _video_param_get(_main_context_t *context)
{
    hy_s32_t ret = -1;
    HyIpcProcessVideoParamGet_s video_param_get;
    HyIpcProcessVideoParamGetResult_s video_param_get_ret;

    HY_MEMSET(&video_param_get, sizeof(video_param_get));
    HY_MEMSET(&video_param_get_ret, sizeof(video_param_get_ret));

    ret = HyIpcProcessDataSync_m(context->ipc_process_client_h,
            HY_IPC_PROCESS_MSG_ID_SYNC_VIDEO_PARAM_GET,
            &video_param_get, &video_param_get_ret, 3);
    if (0 != ret) {
        LOGE("HyIpcProcessDataSync_m failed, id: %d \n", ret);
        return ;
    }

    LOGD("width: %d \n",            video_param_get_ret.width);
    LOGD("height: %d \n",           video_param_get_ret.height);

    LOGD("get video \n");
}

static void _video_param_set(_main_context_t *context)
{
    hy_s32_t ret = -1;
    HyIpcProcessVideoParamSet_s video_param_set;
    HyIpcProcessVideoParamSetResult_s video_param_set_ret;

    HY_MEMSET(&video_param_set, sizeof(video_param_set));
    HY_MEMSET(&video_param_set_ret, sizeof(video_param_set_ret));

    video_param_set.width = 1920;
    video_param_set.height = 1080;

    ret = HyIpcProcessDataSync_m(context->ipc_process_client_h,
            HY_IPC_PROCESS_MSG_ID_SYNC_VIDEO_PARAM_SET,
            &video_param_set, &video_param_set_ret, 3);
    if (0 != ret) {
        LOGE("HyIpcProcessDataSync_m failed, id: %d \n", ret);
        return ;
    }

    LOGD("set video \n");
}

static hy_s32_t _ipcst_video_thread_cb(void *args)
{
    _main_context_t *context = args;

    sleep(1);
    _video_param_get(context);
    _video_param_set(context);
    _video_param_get(context);

    return -1;
}

int main(int argc, char *argv[])
{
    _main_context_t *context = _module_create();
    if (!context) {
        LOGE("_module_create faild \n");
        return -1;
    }

    LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    context->audio_thread_h = HyThreadCreate_m("HYIPCST_audio",
            _ipcst_audio_thread_cb, context);
    if (!context->audio_thread_h) {
        LOGE("hy thread create m failed \n");
    }

    context->video_thread_h = HyThreadCreate_m("HYIPCST_video",
            _ipcst_video_thread_cb, context);
    if (!context->video_thread_h) {
        LOGE("hy thread create m failed \n");
    }

    while (!context->exit_flag) {
        sleep(1);
    }

    HyThreadDestroy(&context->audio_thread_h);
    HyThreadDestroy(&context->video_thread_h);

    _module_destroy(&context);

    return 0;
}

