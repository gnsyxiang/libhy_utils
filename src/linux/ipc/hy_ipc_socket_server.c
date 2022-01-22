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
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "ipc_socket_private.h"
#include "hy_ipc_socket_server.h"

typedef struct {
    hy_ipc_socket_s         socket; //@note: 一定要放在前面，用于指针强制类型转换

    hy_s32_t                pipe_fd[2];
    hy_s32_t                exit_flag:1;
    hy_s32_t                reserved;
} _ipc_socket_server_context_t;

hy_s32_t hy_ipc_server_accept(void *handle,
        HyIpcSocketAcceptCb_t accept_cb, void *args)
{
    LOGT("handle: %p, accept_cb: %p, args: %p \n", handle, accept_cb, args);
    HY_ASSERT_RET_VAL(!handle || !accept_cb, -1);

    hy_s32_t fd;
    fd_set read_fs;
    hy_ipc_socket_s *new_socket = NULL;
    _ipc_socket_server_context_t *context = handle;
    hy_ipc_socket_s *socket = &context->socket;

    if (listen(socket->fd, SOMAXCONN) < 0) {
        LOGES("listen failed, fd: %d, ipc_name: %s \n",
                socket->fd, socket->ipc_name);
        return -1;
    }

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);
        FD_SET(socket->fd, &read_fs);
        FD_SET(context->pipe_fd[0], &read_fs);

        if (select(FD_SETSIZE, &read_fs, NULL, NULL, NULL) < 0) {
            LOGES("select failed \n");
            break;
        }

        if (FD_ISSET(context->pipe_fd[0], &read_fs)) {
            // char buf;
            // read(context->pipe_fd[0], &buf, sizeof(buf));

            LOGW("pipe break while for accept\n");
            break;
        }

        if (FD_ISSET(socket->fd, &read_fs)) {
            fd = accept(socket->fd, NULL, NULL);
            if (fd < 0) {
                LOGES("accept failed \n");
                break;
            }

            new_socket = ipc_socket_create(socket->ipc_name,
                    HY_IPC_SOCKET_TYPE_CLIENT);
            if (!new_socket) {
                LOGE("ipc_socket_create failed \n");
                break;
            }

            new_socket->fd = fd;

            LOGI("accept new client socket, socket: %p, ipc_name: %s, fd: %d \n",
                    new_socket, new_socket->ipc_name, new_socket->fd);

            accept_cb(new_socket, args);
        }
    }

    LOGI("accept stop \n");

    return -1;
}

void hy_ipc_server_destroy(void **context_pp)
{
    LOGT("&context: %p, context: %p \n", context_pp, *context_pp);
    HY_ASSERT_RET(!context_pp || !*context_pp);

    _ipc_socket_server_context_t *context = *context_pp;
    hy_ipc_socket_s *ipc_socket = &context->socket;

    context->exit_flag = 1;
    write(context->pipe_fd[1], context, sizeof(*context));

    usleep(10 * 1000);

    close(context->pipe_fd[0]);
    close(context->pipe_fd[1]);

    close(ipc_socket->fd);

    LOGI("ipc socket sserver create, context: %p, fd: %d, pipe_fd[0]: %d, pipe_fd[1]: %d \n",
            context, ipc_socket->fd, context->pipe_fd[0], context->pipe_fd[1]);
    HY_MEM_FREE_PP(context_pp);
}

void *hy_ipc_server_create(const char *ipc_name, HyIpcSocketType_e type)
{
    LOGT("ipc_name: %s, type: %d \n", ipc_name, type);
    HY_ASSERT_RET_VAL(!ipc_name, NULL);

    hy_u32_t addr_len;
    struct sockaddr_un addr;
    char ipc_path[HY_IPC_SOCKET_NAME_LEN_MAX] = {0};
    _ipc_socket_server_context_t *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_socket_server_context_t *, sizeof(*context));

        context->socket.type = type;
        HY_MEMCPY(context->socket.ipc_name, ipc_name, HY_STRLEN(ipc_name));

        if (0 != pipe(context->pipe_fd)) {
            LOGES("pipe failed \n");
            break;
        }

        hy_ipc_socket_s *ipc_socket = &context->socket;

        ipc_socket->fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (ipc_socket->fd < 0) {
            LOGES("socket failed \n");
            break;
        }

        HY_IPC_SOCKADDR_UN_INIT_(addr, addr_len, ipc_name);

        snprintf(ipc_path, HY_IPC_SOCKET_NAME_LEN_MAX,
                "%s/%s", HY_IPC_SOCKET_PATH_, ipc_socket->ipc_name);
        if (0 == access(ipc_path, F_OK)) {
            remove(ipc_path);
        }

        if (bind(ipc_socket->fd, (const struct sockaddr *)&addr, addr_len) < 0) {
            LOGES("bind failed \n");
            break;
        }

        LOGI("ipc socket sserver create, context: %p, fd: %d, pipe_fd[0]: %d, pipe_fd[1]: %d \n",
                context, ipc_socket->fd, context->pipe_fd[0], context->pipe_fd[1]);
        return context;
    } while (0);

    LOGE("ipc socket server create failed \n");
    hy_ipc_server_destroy((void **)&context);
    return NULL;
}
