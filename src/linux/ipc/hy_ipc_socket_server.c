/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket_server.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 09:34
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 09:34
 */
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"

#include "hy_ipc_socket_server.h"

hy_s32_t hy_ipc_server_accept(hy_ipc_socket_context_s *context,
        HyIpcSocketAcceptCb_t accept_cb, void *args)
{
    LOGT("context: %p, accept_cb: %p \n", context, accept_cb);
    HY_ASSERT_RET_VAL(!context || !accept_cb, -1);

    hy_s32_t fd;
    fd_set read_fs;
    hy_ipc_socket_s *new_socket = NULL;
    hy_ipc_socket_s *socket = context->socket;

    if (listen(socket->fd, SOMAXCONN) < 0) {
        LOGES("listen failed, fd: %d, ipc_name: %s \n",
                socket->fd, socket->ipc_name);
        return -1;
    }

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);
        FD_SET(socket->fd, &read_fs);

        if (select(FD_SETSIZE, &read_fs, NULL, NULL, NULL) < 0) {
            LOGES("select failed \n");
            return -1;
        }

        if (FD_ISSET(socket->fd, &read_fs)) {
            fd = accept(socket->fd, NULL, NULL);
            if (fd < 0) {
                LOGES("accept failed \n");
                return -1;
            }

            new_socket = hy_ipc_socket_socket_create(socket->ipc_name,
                    HY_IPC_SOCKET_TYPE_CLIENT);
            if (!new_socket) {
                LOGE("ipc socket socket create failed \n");
                break;
            }

            new_socket->fd = fd;

            LOGI("accept new socket, fd: %d \n", new_socket->fd);

            accept_cb(&new_socket, args);
        }
    }

    return 0;
}

void hy_ipc_server_destroy(hy_ipc_socket_context_s **context_pp)
{
    LOGT("&context: %p, context: %p \n", context_pp, *context_pp);
    HY_ASSERT_RET(!context_pp || !*context_pp);

    hy_ipc_socket_context_s *context = *context_pp;
    hy_ipc_socket_s *socket = context->socket;

    LOGI("ipc socket server destroy, fd: %d \n", socket->fd);

    close(socket->fd);

    hy_ipc_socket_socket_destroy(&socket);
}

hy_s32_t hy_ipc_server_create(hy_ipc_socket_context_s *context,
        const char *ipc_name, HyIpcSocketType_e type)
{
    LOGT("context: %p, ipc_name: %s, type: %d \n", context, ipc_name, type);
    HY_ASSERT_RET_VAL(!context || !ipc_name, -1);

    hy_u32_t addr_len;
    struct sockaddr_un addr;
    char ipc_path[HY_IPC_SOCKET_PATH_LEN_MAX_] = {0};

    do {
        context->socket = hy_ipc_socket_socket_create(ipc_name, type);
        if (!context->socket) {
            LOGE("socket create failed \n");
            break;
        }

        context->socket->fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (context->socket->fd < 0) {
            LOGES("socket failed \n");
            break;
        }

        HY_IPC_SOCKADDR_UN_INIT_(addr, addr_len, ipc_name);

        snprintf(ipc_path, HY_IPC_SOCKET_PATH_LEN_MAX_,
                "%s/%s", HY_IPC_SOCKET_PATH_, context->socket->ipc_name);
        if (0 == access(ipc_path, F_OK)) {
            remove(ipc_path);
        }

        if (bind(context->socket->fd,
                    (const struct sockaddr *)&addr, addr_len) < 0) {
            LOGES("bind failed \n");
            break;
        }

        LOGI("ipc socket server create, fd: %d \n", context->socket->fd);
        return 0;
    } while (0);

    LOGE("ipc socket server create failed \n");
    hy_ipc_server_destroy(&context);
    return -1;
}
