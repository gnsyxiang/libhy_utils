/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_epoll_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    25/10 2021 19:56
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        25/10 2021      create the file
 * 
 *     last modified: 25/10 2021 19:56
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "hy_type.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_signal.h"
#include "hy_module.h"
#include "hy_utils.h"
#include "hy_epoll.h"

#define _APP_NAME "hy_epoll_demo"

typedef struct {
    hy_s32_t    is_exit;

    void        *epoll_handle;
} _main_epoll_s;

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_epoll_s *context = args;
    context->is_exit = 1;
}

static void _signal_user_cb(void *args)
{
    LOGW("------user cb\n");

    _main_epoll_s *context = args;
    context->is_exit = 1;
}

static void _epoll_event_cb(HyEpollEventCbParam_s *event_cb_param)
{
    switch (event_cb_param->fd_type) {
        case HY_EPOLL_FD_TYPE_FILE:
            break;
        case HY_EPOLL_FD_TYPE_CAN:
            break;
        case HY_EPOLL_FD_TYPE_CLIENT:
            break;
        case HY_EPOLL_FD_TYPE_SERVER:
            break;
        default:
            LOGE("error epoll event fd_type \n");
            break;
    }
}

static void _bool_module_destroy(void)
{
    HyModuleDestroyBool_s bool_module[] = {
        {"signal",          HySignalDestroy },
        {"log",             HyLogDeInit     },
    };

    HY_MODULE_RUN_DESTROY_BOOL(bool_module);
}

static hy_s32_t _bool_module_create(_main_epoll_s *context)
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
    HY_MEMSET(&signal_c, sizeof(signal_c));
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

static void _handle_module_destroy(_main_epoll_s *context)
{
    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"epoll",      &context->epoll_handle,     HyEpollDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_epoll_s *context)
{

    HyEpollConfig_s epoll_c;
    
    HY_MEMSET(&epoll_c, sizeof(epoll_c));
    epoll_c.save_c.max_event        = 128;
    epoll_c.save_c.event_epoll_cb   = _epoll_event_cb;

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"epoll",        &context->epoll_handle,       &epoll_c,            (HyModuleCreateHandleCb_t)HyEpollCreate,     HyEpollDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

int main(int argc, char *argv[])
{
    _main_epoll_s *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_epoll_s *, sizeof(*context));

        if (0 != _bool_module_create(context)) {
            printf("_bool_module_create failed \n");
            break;
        }

        if (0 != _handle_module_create(context)) {
            LOGE("_handle_module_create failed \n");
            break;
        }

        LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

        while (!context->is_exit) {
            sleep(1);
        }
    } while (0);

    _handle_module_destroy(context);
    _bool_module_destroy();
    HY_MEM_FREE_PP(&context);

    return 0;
}

