/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_net.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    16/12 2021 18:54
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        16/12 2021      create the file
 * 
 *     last modified: 16/12 2021 18:54
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_NET_H_
#define __LIBHY_UTILS_INCLUDE_HY_NET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

hy_s32_t HyNetIsConnect(const char *dst);

#ifdef __cplusplus
}
#endif

#endif
