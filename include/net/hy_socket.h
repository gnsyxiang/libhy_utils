/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    05/05 2023 10:31
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        05/05 2023      create the file
 * 
 *     last modified: 05/05 2023 10:31
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_SOCKET_H_
#define __LIBHY_UTILS_INCLUDE_HY_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_type.h"

hy_s32_t HySocketClientWriteOnce(const char *ip, hy_u16_t port,
                                 void *buf, hy_u32_t len);

#ifdef __cplusplus
}
#endif

#endif

