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

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#include "hy_net.h"

#define _APP_NAME "hy_net_demo"

typedef struct {
    void        *log_h;
    void        *signal_h;

    void        *net_h;

    hy_s32_t    exit_flag;
} _main_context_t;

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
        {"net",         &context->net_h,        HyNetDestroy},
        {"signal",      &context->signal_h,     HySignalDestroy},
        {"log",         &context->log_h,        HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_s log_c;
    log_c.save_c.buf_len_min  = 512;
    log_c.save_c.buf_len_max  = 512;
    log_c.save_c.level        = HY_LOG_LEVEL_TRACE;
    log_c.save_c.color_enable = HY_TYPE_FLAG_ENABLE;

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

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",         &context->log_h,        &log_c,         (create_t)HyLogCreate,          HyLogDestroy},
        {"signal",      &context->signal_h,     &signal_c,      (create_t)HySignalCreate,       HySignalDestroy},
        {"net",         &context->net_h,        &net_c,         (create_t)HyNetCreate,          HyNetDestroy},
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

