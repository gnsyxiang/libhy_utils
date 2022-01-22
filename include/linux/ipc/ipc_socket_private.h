/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_socket_private.h
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
#ifndef __LIBHY_UTILS_INCLUDE_IPC_SOCKET_PRIVATE_H_
#define __LIBHY_UTILS_INCLUDE_IPC_SOCKET_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include "hy_ipc_socket.h"

#define HY_IPC_SOCKET_PATH_     "/tmp"

#define HY_IPC_SOCKADDR_UN_INIT_(addr, addr_len, ipc_name)                      \
    do {                                                                        \
        char ipc_path[HY_IPC_SOCKET_NAME_LEN_MAX] = {0};                        \
        addr.sun_family = AF_UNIX;                                              \
        snprintf(ipc_path, HY_IPC_SOCKET_NAME_LEN_MAX,                          \
                "%s/%s", HY_IPC_SOCKET_PATH_, ipc_name);                        \
        strcpy(addr.sun_path, ipc_path);                                        \
        \
        addr_len = strlen(ipc_path) + offsetof(struct sockaddr_un, sun_path);   \
    } while (0)

typedef struct {
    char                        ipc_name[HY_IPC_SOCKET_NAME_LEN_MAX / 2];
    hy_s32_t                    fd;
    HyIpcSocketType_e           type:2;
    hy_s32_t                    reserved;
} hy_ipc_socket_s;

void *ipc_socket_create(const char *ipc_name, HyIpcSocketType_e type);
void ipc_socket_destroy(void **handle);

#ifdef __cplusplus
}
#endif

#endif
