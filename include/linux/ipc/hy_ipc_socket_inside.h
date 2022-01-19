/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket_inside.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 09:17
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 09:17
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_IPC_SOCKET_INSIDE_H_
#define __LIBHY_UTILS_INCLUDE_HY_IPC_SOCKET_INSIDE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include "hy_ipc_socket.h"

typedef struct {
    const char                  *name;
    hy_s32_t                    fd;
    pthread_mutex_t             mutex;
} hy_ipc_socket_s;

typedef struct {
    HyIpcSocketSaveConfig_s     save_config;
    hy_ipc_socket_s             *socket;

    hy_s32_t                    exit_flag:1;
    hy_s32_t                    reserved;
} hy_ipc_socket_context_s;

hy_ipc_socket_s *hy_ipc_socket_socket_create(const char *name);
void hy_ipc_socket_socket_destroy(hy_ipc_socket_s **socket);

#ifdef __cplusplus
}
#endif

#endif
