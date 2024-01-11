/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    net_config.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    02/04 2022 14:18
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        02/04 2022      create the file
 * 
 *     last modified: 02/04 2022 14:18
 */
#ifndef __LIBHY_UTILS_INCLUDE_NET_CONFIG_H_
#define __LIBHY_UTILS_INCLUDE_NET_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_os_type/hy_os_type.h>

#include "hy_net.h"

hy_s32_t net_config_wifi_load(HyNetWifiConfig_s *wifi_config);
hy_s32_t net_config_wifi_save(HyNetWifiConfig_s *wifi_config);

hy_s32_t net_config_eth_load(HyNetEthConfig_s *eth_config);
hy_s32_t net_config_eth_save(HyNetEthConfig_s *eth_config);

#ifdef __cplusplus
}
#endif

#endif

