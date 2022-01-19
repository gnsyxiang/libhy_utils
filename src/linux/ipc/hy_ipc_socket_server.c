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
    LOGT("handle: %p, accept_cb: %p \n", context, accept_cb);
    HY_ASSERT_VAL_RET_VAL(!context || !accept_cb, -1);

    hy_ipc_socket_s *socket = context->socket;
    fd_set read_fs;
    hy_s32_t fd;

    if (listen(socket->fd, SOMAXCONN) < 0) {
        LOGES("listen failed, fd: %d, name: %s \n", socket->fd, socket->name);
        return -1;
    }

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);
        FD_SET(socket->fd, &read_fs);

        if (select(FD_SETSIZE, &read_fs, NULL, NULL, NULL) < 0) {
            LOGES("select failed \n");
            break;
        }

        if (FD_ISSET(socket->fd, &read_fs)) {
            fd = accept(socket->fd, NULL, NULL);
            if (fd < 0) {
                LOGES("accept failed \n");
                return -1;
            }

            accept_cb(fd, args);
        }
    }

    return 0;
}

void hy_ipc_server_destroy(hy_ipc_socket_context_s **context_pp)
{
    LOGT("handle: %p, *handle: %p \n", context_pp, *context_pp);
    HY_ASSERT_VAL_RET(!context_pp || !*context_pp);

    hy_ipc_socket_context_s *context = *context_pp;
    hy_ipc_socket_s *socket = context->socket;

    close(socket->fd);

    LOGI("socket server destroy, handle: %p, fd: %d \n",
            context->socket, context->socket->fd);
    hy_ipc_socket_socket_destroy(&socket);
}

hy_s32_t hy_ipc_server_create(hy_ipc_socket_context_s *context)
{
    LOGT("handle: %p \n", context);
    HY_ASSERT_VAL_RET_VAL(!context, -1);

    hy_s32_t fd;
    hy_u32_t addr_len;
    struct sockaddr_un addr;
    HyIpcSocketSaveConfig_s *save_config = &context->save_config;

    do {
        context->socket = hy_ipc_socket_socket_create(save_config->server_name);
        if (!context->socket) {
            LOGE("socket create failed \n");
            break;
        }

        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd < 0) {
            LOGES("socket failed \n");
            break;
        }

        context->socket->fd = fd;

        unlink(save_config->server_name);

        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, save_config->server_name);

        addr_len = strlen(save_config->server_name)
            + offsetof(struct sockaddr_un, sun_path);
        if (bind(fd, (const struct sockaddr *)&addr, addr_len) < 0) {
            LOGES("bind failed \n");
            break;
        }

        LOGI("socket server create, handle: %p, fd: %d \n", context->socket, fd);
        return 0;
    } while (0);

    hy_ipc_server_destroy(&context);
    return -1;
}
