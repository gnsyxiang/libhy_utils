/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_thread_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    05/05 2023 15:52
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        05/05 2023      create the file
 * 
 *     last modified: 05/05 2023 15:52
 */
#include <stdio.h>

#include <hy_log/hy_log.h>

#include "config.h"

#include "hy_module.h"
#include "hy_signal.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_utils.h"

#include "hy_thread.h"

#define _APP_NAME "hy_thread_demo"

typedef struct {
    hy_s32_t    is_exit;

    HyThread_s  *normal_thread_h;
    HyThread_s  *low_thread_h;
    HyThread_s  *high_thread_h;
} _main_context_s;

static hy_s32_t _normal_loop_cb(void *args)
{
    _main_context_s *context = args;

    while (!context->is_exit) {
        LOGI("normal \n");
        sleep(1);
    }

    return -1;
}

static hy_s32_t _rt_loop_cb(void *args)
{
    char *label = args;
    hy_s32_t cnt = 0;

    HyThreadAttachCPU(0);

    for (size_t i = 0; i < 10; i++) {
        for (size_t j = 0; j < 100; j++) {
            if (j % 20 == 0) {
                printf("label: %s, cnt: %d \n", label, cnt++);
            }
        }
    }

    return -1;
}

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

static hy_s32_t _bool_module_create(_main_context_s *context)
{
    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(log_c));
    log_c.config_file               = "../res/hy_log/zlog.conf";
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.level              = HY_LOG_LEVEL_INFO;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL_NO_PID_ID;

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

static void _handle_module_destroy(_main_context_s **context_pp)
{
    _main_context_s *context = *context_pp;

    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"normal",  (void **)&context->normal_thread_h, (HyModuleDestroyHandleCb_t)HyThreadDestroy},
        {"low",     (void **)&context->low_thread_h,    (HyModuleDestroyHandleCb_t)HyThreadDestroy},
        {"high",    (void **)&context->high_thread_h,   (HyModuleDestroyHandleCb_t)HyThreadDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_s *context)
{
    HyThreadConfig_s normal_thread_c;
    const char *normal_thread_name          = "normal_thread";
    HY_MEMSET(&normal_thread_c, sizeof(normal_thread_c));
    normal_thread_c.save_c.thread_loop_cb   = _normal_loop_cb;
    normal_thread_c.save_c.args             = context;
    HY_STRNCPY(normal_thread_c.save_c.name, HY_THREAD_NAME_LEN_MAX,
               normal_thread_name, HY_STRLEN(normal_thread_name));

    HyThreadConfig_s low_thread_c;
    const char *low_thread_name             = "low_thread";
    HY_MEMSET(&low_thread_c, sizeof(low_thread_c));
    low_thread_c.save_c.policy              = HY_THREAD_POLICY_SCHED_FIFO;
    low_thread_c.save_c.priority            = 10;
    low_thread_c.save_c.thread_loop_cb      = _rt_loop_cb;
    low_thread_c.save_c.args                = (void *)"low";
    HY_STRNCPY(low_thread_c.save_c.name, HY_THREAD_NAME_LEN_MAX,
               low_thread_name, HY_STRLEN(low_thread_name));

    HyThreadConfig_s high_thread_c;
    const char *high_thread_name            = "high_thread";
    HY_MEMSET(&high_thread_c, sizeof(high_thread_c));
    high_thread_c.save_c.policy             = HY_THREAD_POLICY_SCHED_FIFO;
    high_thread_c.save_c.priority           = 20;
    high_thread_c.save_c.thread_loop_cb     = _rt_loop_cb;
    high_thread_c.save_c.args               = (void *)"high";
    HY_STRNCPY(high_thread_c.save_c.name, HY_THREAD_NAME_LEN_MAX,
               high_thread_name, HY_STRLEN(high_thread_name));

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"normal",  (void **)&context->normal_thread_h, &normal_thread_c,   (HyModuleCreateHandleCb_t)HyThreadCreate, (HyModuleDestroyHandleCb_t)HyThreadDestroy},
        {"low",     (void **)&context->low_thread_h,    &low_thread_c,      (HyModuleCreateHandleCb_t)HyThreadCreate, (HyModuleDestroyHandleCb_t)HyThreadDestroy},
        {"high",    (void **)&context->high_thread_h,   &high_thread_c,     (HyModuleCreateHandleCb_t)HyThreadCreate, (HyModuleDestroyHandleCb_t)HyThreadDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

int main(int argc, char *argv[])
{
    _main_context_s *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_s *, sizeof(*context));

        struct {
            const char *name;
            hy_s32_t (*create)(_main_context_s *context);
        } _create_arr[] = {
            {"_bool_module_create",     _bool_module_create},
            {"_handle_module_create",   _handle_module_create},
        };
        for (size_t i = 0; i < HY_UTILS_ARRAY_CNT(_create_arr); i++) {
            if (_create_arr[i].create) {
                if (0 != _create_arr[i].create(context)) {
                    LOGE("%s failed \n", _create_arr[i].name);
                }
            }
        }

        LOGE("version: %s, data: %s, time: %s \n", VERSION, __DATE__, __TIME__);

        while (!context->is_exit) {
            sleep(1);
        }
    } while (0);

    void (*destroy_arr[])(_main_context_s **context_pp) = {
        _handle_module_destroy,
        _bool_module_destroy
    };
    for (size_t i = 0; i < HY_UTILS_ARRAY_CNT(destroy_arr); i++) {
        if (destroy_arr[i]) {
            destroy_arr[i](&context);
        }
    }
    HY_MEM_FREE_PP(&context);

    return 0;
}
