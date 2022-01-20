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

#define HY_IPC_SOCKET_PATH_LEN_MAX_     (64)
#define HY_IPC_SOCKET_PATH_             "/tmp"

typedef struct {
    const char                  *ipc_name;
    hy_s32_t                    fd;
    pthread_mutex_t             mutex;
} hy_ipc_socket_s;

typedef struct {
    HyIpcSocketSaveConfig_s     save_config;
    hy_ipc_socket_s             *socket;

    hy_s32_t                    exit_flag:1;
    hy_s32_t                    reserved;
} hy_ipc_socket_context_s;

hy_ipc_socket_s *hy_ipc_socket_socket_create(const char *ipc_name);
void hy_ipc_socket_socket_destroy(hy_ipc_socket_s **socket);

#define HY_IPC_SOCKADDR_UN_INIT_(type, addr, addr_len, ipc_name)                \
    do {                                                                        \
        char ipc_path[HY_IPC_SOCKET_PATH_LEN_MAX_] = {0};                       \
        addr.sun_family = AF_UNIX;                                              \
        snprintf(ipc_path, HY_IPC_SOCKET_PATH_LEN_MAX_,                         \
                "%s/%s", HY_IPC_SOCKET_PATH_, ipc_name);                        \
        strcpy(addr.sun_path, ipc_path);                                        \
        \
        addr_len = strlen(ipc_path) + offsetof(struct sockaddr_un, sun_path);   \
        \
        if (type == HY_IPC_SOCKET_TYPE_SERVER) {                                \
            unlink(ipc_path);                                                   \
        }                                                                       \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif
