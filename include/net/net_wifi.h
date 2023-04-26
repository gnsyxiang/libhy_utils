/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    net_wifi.h
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
#ifndef __LIBHY_UTILS_INCLUDE_NET_WIFI_H_
#define __LIBHY_UTILS_INCLUDE_NET_WIFI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_net.h"

typedef struct {
    HyNetWifiConfig_s       *wifi_c;
    HyNetIpInfo_s           *wifi_ip_info;
    HyGpio_s                gpio;
} NetWifiSaveConfig_t;

typedef struct {
    NetWifiSaveConfig_t     save_c;
} NetWifiConfig_t;

void *net_wifi_create(NetWifiConfig_t *net_wifi_c);
void net_wifi_destroy(void **handle);

#define net_wifi_create_m(_gpio, _wifi_c, _wifi_ip_info)                \
    ({                                                                  \
        NetWifiConfig_t net_wifi_c;                                     \
        HY_MEMSET(&net_wifi_c, sizeof(net_wifi_c));                     \
        HY_MEMCPY(&net_wifi_c.save_c.gpio, _gpio, sizeof(*_gpio));      \
        net_wifi_c.save_c.wifi_c        = _wifi_c;                      \
        net_wifi_c.save_c.wifi_ip_info  = _wifi_ip_info;                \
        net_wifi_create(&net_wifi_c);                                   \
     })

#ifdef __cplusplus
}
#endif

#endif

