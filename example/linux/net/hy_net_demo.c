/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_net_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    02/04 2022 11:03
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        02/04 2022      create the file
 * 
 *     last modified: 02/04 2022 11:03
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hy_os/hy_assert.h>
#include <hy_os/hy_mem.h>
#include <hy_os/hy_string.h>
#include <hy_os/hy_module.h>
#include <hy_os/hy_utils.h>
#include <hy_os/hy_signal.h>

#include "config.h"

#include "hy_net.h"

#define _APP_NAME "hy_net_demo"

typedef struct {
    void        *net_h;

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
    HY_MEMSET(&log_c, sizeof(HyLogConfig_s));
    log_c.port                      = 56789;
    log_c.fifo_len                  = 10 * 1024;
    log_c.config_file               = "../res/hy_log/zlog.conf";
    log_c.save_c.level              = HY_LOG_LEVEL_INFO;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL_NO_PID_ID;

    hy_s8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    hy_s8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
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

static void _eth_state_cb(HyNetState_e state, void *args)
{

}

static void _eth_set_default_cb(HyNetEthConfig_s *eth_c,
        HyNetIpInfo_s *ip_info, void *args)
{

}

static void _wifi_power_gpio_cb(HyGpio_s *gpio)
{

}

static void _wifi_set_default_cb(HyNetWifiConfig_s *wifi_c,
        HyNetIpInfo_s *ip_info, void *args)
{
    HY_ASSERT_RET(!wifi_c);

    HY_STRCPY(wifi_c->name,         "wlan0");
    HY_STRCPY(wifi_c->ssid,         "ipctest");
    HY_STRCPY(wifi_c->pwd,          "12345678");
    HY_STRCPY(wifi_c->driver_name,  "wext");

    wifi_c->enable = 1;
    wifi_c->dhcp = 0;

    ip_info->ip = 3232235880;
    ip_info->gw = 3232235777;
    ip_info->mask = 4294967040;
    ip_info->dns1 = 3232235777;
    ip_info->dns2 = 1920103026;
}

static void _wifi_state_cb(HyNetState_e state, void *args)
{

}

static void _wifi_scan_result_cb(HyNetWifiScanInfo_s *scan_info, void *args)
{

}

static void _handle_module_destroy(_main_context_s **context_pp)
{
    _main_context_s *context = *context_pp;

    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"net",         &context->net_h,        HyNetDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_s *context)
{
    HyNetConfig_s net_c;
    HY_MEMSET(&net_c, sizeof(net_c));
    #define _NET_WIFI_CONFIG_PATH "./net_wifi_config.json"
    HY_STRNCPY(net_c.save_c.config_path, HY_NET_CONFIG_PATH_LEN_MAX,
            _NET_WIFI_CONFIG_PATH, HY_STRLEN(_NET_WIFI_CONFIG_PATH));
    net_c.save_c.scan_result_cb = _wifi_scan_result_cb;
    net_c.save_c.wifi_state_cb  = _wifi_state_cb;
    net_c.save_c.eth_state_cb   = _eth_state_cb;
    net_c.save_c.args           = context;
    net_c.wifi_set_default_cb   = _wifi_set_default_cb;
    net_c.eth_set_default_cb    = _eth_set_default_cb;
    net_c.wifi_power_gpio       = _wifi_power_gpio_cb;
    net_c.args                  = context;

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"net",         &context->net_h,        &net_c,         (HyModuleCreateHandleCb_t)HyNetCreate,          HyNetDestroy},
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
        for (hy_u32_t i = 0; i < HY_UTILS_ARRAY_CNT(create_arr); i++) {
            if (create_arr[i].create) {
                if (0 != create_arr[i].create(context)) {
                    LOGE("%s failed \n", create_arr[i].name);
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
    for (hy_u32_t i = 0; i < HY_UTILS_ARRAY_CNT(destroy_arr); i++) {
        if (destroy_arr[i]) {
            destroy_arr[i](&context);
        }
    }
    HY_MEM_FREE_PP(&context);

    return 0;
}

