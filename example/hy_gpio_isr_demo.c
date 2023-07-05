/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_gpio_isr_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    16/05 2023 11:53
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        16/05 2023      create the file
 * 
 *     last modified: 16/05 2023 11:53
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "hy_type.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_signal.h"
#include "hy_module.h"
#include "hy_utils.h"
#include "hy_gpio.h"
#include "hy_time.h"

#include "hy_gpio_isr.h"

#define _APP_NAME "hy_gpio_isr_demo"

#define _SYNC_KEY_GPIO          (42)
#define _TEST_SYNC_KEY_HZ       (40)
#define _TEST_SYNC_KEY_MS       (1000 / _TEST_SYNC_KEY_HZ)

typedef struct {
    hy_s32_t            is_exit;

    HyGpioIsr_s   *gpio_isr_h;
    struct timeval      sync_key_time;
    hy_u32_t            sync_key_cnt;
    hy_u32_t            sync_key_spend_time;
} _main_context_s;

static void _sync_key_cb(hy_s32_t level, void *args)
{
    _main_context_s *context = args;

    if (level == '1') {
        struct timeval newtime;
        struct timeval difference;
        gettimeofday(&newtime, NULL);
        HY_TIME_TIMEVAL_SUB(&newtime, &context->sync_key_time, &difference);
        context->sync_key_time = newtime;
        context->sync_key_spend_time = difference.tv_usec / 1000;

        context->sync_key_cnt++;

        LOGI("sync key time: %d, sync cnt: %d \n",
             context->sync_key_spend_time, context->sync_key_cnt);
    } else {
        LOGW("----up \n");
    }
}

static void _sync_key_timeout_cb(void *args)
{
    LOGE("----sync key timeout \n");
}

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_context_s *context = args;
    context->is_exit = 1;
}

static void _signal_user_cb(void *args)
{
    LOGW("------user cb\n");

    _main_context_s *context = args;
    context->is_exit = 1;
}

static void _bool_module_destroy(void)
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

static void _handle_module_destroy(_main_context_s *context)
{
    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"gpio_isr",           (void **)&context->gpio_isr_h,      (HyModuleDestroyHandleCb_t)HyGpioIsrDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_s *context)
{
    HyGpioIsrConfig_s gpio_isr_c;
    HY_MEMSET(&gpio_isr_c, sizeof(gpio_isr_c));
    gpio_isr_c.gpio = _SYNC_KEY_GPIO;
    gpio_isr_c.direction = HY_GPIO_DIRECTION_IN;
    gpio_isr_c.active_val = HY_GPIO_ACTIVE_VAL_0;
    gpio_isr_c.trigger = HY_GPIO_TRIGGER_RISING;
    gpio_isr_c.save_c.gpio_isr_cb = _sync_key_cb;
    gpio_isr_c.save_c.args = context;
    gpio_isr_c.save_c.gpio_isr_timeout_cb = _sync_key_timeout_cb;
    gpio_isr_c.save_c.timeout_args = context;
    gpio_isr_c.save_c.timeout_ms = _TEST_SYNC_KEY_MS + 3;

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"gpio_isr",           (void **)&context->gpio_isr_h,    &gpio_isr_c,           (HyModuleCreateHandleCb_t)HyGpioIsrCreate, (HyModuleDestroyHandleCb_t)HyGpioIsrDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

int main(int argc, char *argv[])
{
    _main_context_s *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_s *, sizeof(*context));

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
