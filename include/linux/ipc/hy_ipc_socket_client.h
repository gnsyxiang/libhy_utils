/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket_client.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 16:37
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 16:37
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_IPC_SOCKET_CLIENT_H_
#define __LIBHY_UTILS_INCLUDE_HY_IPC_SOCKET_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_ipc_socket_inside.h"

hy_s32_t hy_ipc_client_create(hy_ipc_socket_context_s *context,
        const char *ipc_name);
void hy_ipc_client_destroy(hy_ipc_socket_context_s **context);

hy_s32_t hy_ipc_client_connect(hy_ipc_socket_context_s *context,
        hy_u32_t timeout_s);

#ifdef __cplusplus
}
#endif

#endif

