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

#include <stdint.h>

void HyBitSet(char *byte, uint32_t index);

int32_t HyBitVal(char *byte, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif

