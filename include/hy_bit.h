/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_bit.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 14:20
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 14:20
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_BIT_H_
#define __LIBHY_UTILS_INCLUDE_HY_BIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_log/hy_type.h>

void HyBitSet(char *byte, hy_u32_t index);

void HyBitReSet(char *byte, hy_u32_t index);

hy_s32_t HyBitGet(char *byte, hy_u32_t index);

#ifdef __cplusplus
}
#endif

#endif

