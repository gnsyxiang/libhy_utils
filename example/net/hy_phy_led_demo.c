/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_phy_led_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 11:14
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 11:14
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "config.h"

#include "hy_module.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_type.h"
#include "hy_signal.h"
#include "hy_utils.h"

#include "hy_phy_led.h"

#define _APP_NAME "hy_phy_led"

typedef struct {
    void        *phy_led_h;

    hy_s32_t    is_exit;
} _main_context_s;

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

static void _handle_module_destroy(_main_context_s **context_pp)
{
    _main_context_s *context = *context_pp;

    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"net",         &context->phy_led_h,        HyPHYLedDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_s *context)
{
    HyPHYLedLed_s led[HY_PHY_LED_NUM_MAX][HY_PHY_LED_MODE_MAX] = {
        {
            {HY_PHY_LED_MODE_OFF,         {{0x1e, 0x40c0}, {0x1f, 0x8000}}},
            {HY_PHY_LED_MODE_ON,          {{0x1e, 0x40c0}, {0x1f, 0xa000}}},
            {HY_PHY_LED_MODE_SLOW_BLINK,  {{0x1e, 0x40c0}, {0x1f, 0xc000}}},
            {HY_PHY_LED_MODE_FAST_BLINK,  {{0x1e, 0x40c0}, {0x1f, 0xe000}}},
        },
        {
            {HY_PHY_LED_MODE_OFF,         {{0x1e, 0x40c3}, {0x1f, 0x8000}}},
            {HY_PHY_LED_MODE_ON,          {{0x1e, 0x40c3}, {0x1f, 0xa000}}},
            {HY_PHY_LED_MODE_SLOW_BLINK,  {{0x1e, 0x40c3}, {0x1f, 0xc000}}},
            {HY_PHY_LED_MODE_FAST_BLINK,  {{0x1e, 0x40c3}, {0x1f, 0xe000}}},
        },
    };

    HyPHYLedConfig_s phy_led_c;
    memset(&phy_led_c, '\0', sizeof(phy_led_c));
    #define _DEV_NAME "eth0"
    HY_STRNCPY(phy_led_c.save_c.dev_name, sizeof(phy_led_c.save_c.dev_name),
            _DEV_NAME, HY_STRLEN(_DEV_NAME));
    HY_MEMCPY(phy_led_c.save_c.led, &led, sizeof(led));

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"net",         &context->phy_led_h,        &phy_led_c,             (HyModuleCreateHandleCb_t)HyPHYLedCreate,       HyPHYLedDestroy},
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
        } create_arr[] = {
            {"_bool_module_create",     _bool_module_create},
            {"_handle_module_create",   _handle_module_create},
        };
        for (size_t i = 0; i < HY_UTILS_ARRAY_CNT(create_arr); i++) {
            if (create_arr[i].create) {
                if (0 != create_arr[i].create(context)) {
                    LOGE("%s failed \n", create_arr[i].name);
                }
            }
        }

        LOGE("version: %s, data: %s, time: %s \n", VERSION, __DATE__, __TIME__);

        HyPHYLedSetLed(HY_PHY_LED_NUM_0, HY_PHY_LED_MODE_SLOW_BLINK);
        HyPHYLedSetLed(HY_PHY_LED_NUM_1, HY_PHY_LED_MODE_FAST_BLINK);

        while (!context->is_exit) {
            // HyNetLedSetLed(HY_NET_LED_LED_0, HY_NET_LED_LED_MODE_ON);
            // HyNetLedSetLed(HY_NET_LED_LED_1, HY_NET_LED_LED_MODE_OFF);
            // usleep(100 * 1000);
            //
            // HyNetLedSetLed(HY_NET_LED_LED_0, HY_NET_LED_LED_MODE_OFF);
            // HyNetLedSetLed(HY_NET_LED_LED_1, HY_NET_LED_LED_MODE_ON);
            usleep(100 * 1000);
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

