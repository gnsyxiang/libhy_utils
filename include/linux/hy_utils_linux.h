/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_utils_linux.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/03 2022 19:13
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/03 2022      create the file
 * 
 *     last modified: 30/03 2022 19:13
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_UTILS_LINUX_H_
#define __LIBHY_UTILS_INCLUDE_HY_UTILS_LINUX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

/**
 * @brief 生成随机数
 *
 * @param range 限定随机数的范围
 *
 * @return 生成1到range之间的任意一个数
 */
hy_u32_t HyUtilsLinuxRandomNum(hy_u32_t range);

#ifdef __cplusplus
}
#endif

#endif

