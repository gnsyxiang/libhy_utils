/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    net_wifi_config.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    01/04 2022 13:50
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        01/04 2022      create the file
 * 
 *     last modified: 01/04 2022 13:50
 */
#ifndef __LIBHY_UTILS_INCLUDE_NET_WIFI_CONFIG_H_
#define __LIBHY_UTILS_INCLUDE_NET_WIFI_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

#define NET_WIFI_CONFIG_DEV_NAME_LEN_MAX    (8)
#define NET_WIFI_CONFIG_SSID_LEN_MAX        (32)
#define NET_WIFI_CONFIG_PWD_LEN_MAX         (32)

typedef struct {
    char        name[NET_WIFI_CONFIG_DEV_NAME_LEN_MAX];
    hy_s32_t    enable;
    hy_s32_t    dhcp;

    hy_u32_t    ip;
    hy_u32_t    mask;
    hy_u32_t    gw;
    hy_u32_t    dns1;
    hy_u32_t    dns2;

    char        ssid[NET_WIFI_CONFIG_SSID_LEN_MAX];
    char        pwd[NET_WIFI_CONFIG_PWD_LEN_MAX];

    hy_u32_t    wpa_version;
    hy_s32_t    encrypt;
} net_wifi_config_s;

typedef void (*net_wifi_config_set_default_cb_t)(
        net_wifi_config_s *net_wifi_c, void *args);

typedef struct {
    net_wifi_config_set_default_cb_t    set_default_cb;
    void                                *args;
    const char                          *file_path;
} net_wifi_config_save_config_s;

typedef struct {
    net_wifi_config_save_config_s       save_c;
} net_wifi_config_config_s;

void *net_wifi_config_create(net_wifi_config_config_s *net_wifi_config_c);
void net_wifi_config_destroy(void **handle);

hy_s32_t net_wifi_config_load(void *handle, net_wifi_config_s *net_wifi_c);
hy_s32_t net_wifi_config_save(void *handle, net_wifi_config_s *net_wifi_c);

#ifdef __cplusplus
}
#endif

#endif

