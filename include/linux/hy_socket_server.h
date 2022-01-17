/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket_server.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 09:23
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 09:23
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_SOCKET_SERVER_H_
#define __LIBHY_UTILS_INCLUDE_HY_SOCKET_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_socket_inside.h"

hy_s32_t hy_server_create(hy_socket_context_s *context);
void hy_server_destroy(hy_socket_context_s **context);

#ifdef __cplusplus
}
#endif

#endif
