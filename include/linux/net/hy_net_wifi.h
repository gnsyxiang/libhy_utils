/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_net_wifi.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    01/04 2022 11:44
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        01/04 2022      create the file
 * 
 *     last modified: 01/04 2022 11:44
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_NET_WIFI_H_
#define __LIBHY_UTILS_INCLUDE_HY_NET_WIFI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

#define NET_WIFI_DEV_NAME_LEN_MAX    (8)
#define NET_WIFI_SSID_LEN_MAX        (32)
#define NET_WIFI_PWD_LEN_MAX         (32)

typedef struct {
    char        name[NET_WIFI_DEV_NAME_LEN_MAX];
    hy_s32_t    enable;
    hy_s32_t    dhcp;

    hy_u32_t    ip;
    hy_u32_t    mask;
    hy_u32_t    gw;
    hy_u32_t    dns1;
    hy_u32_t    dns2;

    char        ssid[NET_WIFI_SSID_LEN_MAX];
    char        pwd[NET_WIFI_PWD_LEN_MAX];

    hy_u32_t    wpa_version;
    hy_s32_t    cipher;
} net_wifi_config_s;

typedef void (*HyNetWifiStartCb_t)(void *args);
typedef void (*HyNetWifiStopCb_t)(void *args);

typedef void (*HyNetWifiSetDefaultCb_t)(net_wifi_config_s *net_wifi_c, void *args);

typedef struct {
    const char                  *name;

    HyNetWifiStartCb_t          start_cb;
    HyNetWifiStopCb_t           stop_cb;
    void                        *args;
} HyNetWifiSaveConfig_t;

typedef struct {
    HyNetWifiSaveConfig_t       save_c;

    HyNetWifiSetDefaultCb_t     set_default_cb;
    void                        *args;
} HyNetWifiConfig_t;

void *HyNetWifiCreate(HyNetWifiConfig_t *net_wifi_c);
void HyNetWifiDestroy(void **handle);

hy_s32_t HyNetWifiIsEnable();

#ifdef __cplusplus
}
#endif

#endif

