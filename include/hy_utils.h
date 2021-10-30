/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_utils.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 10:55
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 10:55
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_UTILS_H_
#define __LIBHY_UTILS_INCLUDE_HY_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HyUtilsMinMacro(x, y) ((x) < (y) ? (x) : (y))

#define HyUtilsIsPowerOf2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))

#ifdef __cplusplus
}
#endif

#endif

