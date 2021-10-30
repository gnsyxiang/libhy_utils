/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_md5sum.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 13:58
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 13:58
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_MD5SUM_H_
#define __LIBHY_UTILS_INCLUDE_HY_MD5SUM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#define MD5SUM_LEN (16)

void HyMd5sum(const uint8_t *msg, size_t msg_len, uint8_t digest[MD5SUM_LEN]);

#ifdef __cplusplus
}
#endif

#endif

