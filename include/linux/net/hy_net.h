/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_net.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    02/04 2022 09:03
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        02/04 2022      create the file
 * 
 *     last modified: 02/04 2022 09:03
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_NET_H_
#define __LIBHY_UTILS_INCLUDE_HY_NET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_compile.h"
#include "hy_hal/hy_gpio.h"

#define HY_NET_CONFIG_PATH_LEN_MAX  (64)
#define HY_NET_DEV_NAME_LEN_MAX     (8)
#define HY_NET_WIFI_SSID_LEN_MAX    (64)
#define HY_NET_WIFI_PWD_LEN_MAX     (64)
#define HY_NET_MAC_LEN_MAX          (24)

typedef enum {
    HY_NET_TYPE_ETH,
    HY_NET_TYPE_WIFI,

    HY_NET_TYPE_MAX,
} HyNetType_e;

typedef enum {
    HY_NET_STATE_NONE,

    HY_NET_STATE_ETH_DISCONNECT,
    HY_NET_STATE_ETH_CONNECT,

    HY_NET_STATE_WIFI_DISCONNECT,
    HY_NET_STATE_WIFI_CONNECT,

    HY_NET_STATE_MAX,
} HyNetState_e;

typedef struct {
    char        name[HY_NET_DEV_NAME_LEN_MAX];
    hy_s32_t    enable;
    hy_s32_t    dhcp;

    hy_u32_t    ip;
    hy_u32_t    mask;
    hy_u32_t    gw;
    hy_u32_t    dns1;
    hy_u32_t    dns2;
} HyNetEthConfig_s;

typedef struct {
    char        name[HY_NET_DEV_NAME_LEN_MAX];
    hy_s32_t    enable;
    hy_s32_t    dhcp;

    hy_u32_t    ip;
    hy_u32_t    mask;
    hy_u32_t    gw;
    hy_u32_t    dns1;
    hy_u32_t    dns2;

    char        ssid[HY_NET_WIFI_SSID_LEN_MAX];
    char        pwd[HY_NET_WIFI_PWD_LEN_MAX];

    hy_u32_t    wpa_version;
    hy_s32_t    cipher;
} HyNetWifiConfig_s;

typedef struct {
    char        name[HY_NET_DEV_NAME_LEN_MAX];
    hy_s32_t    enable;
    hy_s32_t    dhcp;

    hy_u32_t    ip;
    hy_u32_t    mask;
    hy_u32_t    gw;
    hy_u32_t    dns1;
    hy_u32_t    dns2;
} PACKED_4 HyNetParam_s;

typedef struct {
    char        enable;
    char        is_link;
    char        is_dhcp;

    hy_u32_t    ip;
    hy_u32_t    mask;
    hy_u32_t    gw;
    hy_u32_t    dns1;
    hy_u32_t    dns2;

    char        mac[HY_NET_MAC_LEN_MAX];
} PACKED_4 HyNetEthState_s;

typedef struct {
    char        enable;
    char        signal;
    char        is_dhcp;

    hy_u32_t    ip;
    hy_u32_t    mask;
    hy_u32_t    gw;
    hy_u32_t    dns1;
    hy_u32_t    dns2;

    char        ssid[HY_NET_WIFI_SSID_LEN_MAX];
    char        mac[HY_NET_MAC_LEN_MAX];
} PACKED_4 HyNetWifiState_s;

typedef struct {
    char        ssid[HY_NET_WIFI_SSID_LEN_MAX];
    char        pwd[HY_NET_WIFI_PWD_LEN_MAX];
} PACKED_4 HyNetWifiInfo_s;

typedef struct {
    char        ssid[HY_NET_WIFI_SSID_LEN_MAX];
} PACKED_4 HyNetWifiScanInfo_s;

typedef void (*HyNetStateCb_t)(HyNetState_e state, void *args);

typedef void (*HyNetEthSetDefaultCb_t)(HyNetEthConfig_s *eth_c, void *args);

typedef void (*HyNetWifiPowerGpioCb_t)(HyGpio_s *gpio);

typedef void (*HyNetWifiSetDefaultCb_t)(HyNetWifiConfig_s *wifi_c, void *args);

typedef void (*HyNetWifiScanResultCb_t)(HyNetWifiScanInfo_s *scan_info,
        void *args);

typedef struct {
    char                        config_path[HY_NET_CONFIG_PATH_LEN_MAX];

    HyNetStateCb_t              eth_state_cb;

    HyNetStateCb_t              wifi_state_cb;
    HyNetWifiScanResultCb_t     scan_result_cb;

    void                        *args;
} PACKED_4 HyNetSaveConfig_s;

typedef struct {
    HyNetSaveConfig_s           save_c;

    HyNetWifiPowerGpioCb_t      wifi_power_gpio;
    HyNetWifiSetDefaultCb_t     wifi_set_default_cb;
    HyNetEthSetDefaultCb_t      eth_set_default_cb;
    void                        *args;
} PACKED_4 HyNetConfig_s;

void *HyNetCreate(HyNetConfig_s *net_c);
void HyNetDestroy(void **handle);

hy_s32_t HyNetEthParamGet(void *handle, HyNetParam_s *param);
hy_s32_t HyNetEthParamSet(void *handle, HyNetParam_s *param);

hy_s32_t HyNetEthStateGet(void *handle, HyNetEthState_s *eth_state);


hy_s32_t HyNetWifiParamGet(void *handle, HyNetParam_s *param);
hy_s32_t HyNetWifiParamSet(void *handle, HyNetParam_s *param);

hy_s32_t HyNetWifiPowerUp(void *handle);
hy_s32_t HyNetWifiPowerDown(void *handle);

hy_s32_t HyNetWifiScanStart(void *handle);
hy_s32_t HyNetWifiScanStop(void *handle);
hy_s32_t HyNetWifiConnect(void *handle, HyNetWifiInfo_s *wifi_info);

hy_s32_t HyNetWifiStateGet(void *handle, HyNetWifiState_s *wifi_state);

#ifdef __cplusplus
}
#endif

#endif

