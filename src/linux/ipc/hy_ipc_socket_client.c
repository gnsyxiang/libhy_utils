/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket_client.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 16:38
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 16:38
 */
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"

#include "hy_ipc_socket_client.h"

hy_s32_t hy_ipc_client_connect(hy_ipc_socket_context_s *context,
        hy_u32_t timeout_s)
{
    LOGT("handle: %p, timeout_s: %d \n", context, timeout_s);
    HY_ASSERT_VAL_RET_VAL(!context, -1);

    hy_ipc_socket_s *socket = context->socket;
    hy_s32_t ret = 0;
    hy_u32_t time_cnt = 0;
    hy_u32_t addr_len = 0;
    struct sockaddr_un addr;

    HY_IPC_SOCKADDR_UN_INIT_(addr, addr_len, socket->ipc_name);

    do {
        ret = connect(socket->fd, (const struct sockaddr *)&addr, addr_len);
        if (ret < 0) {
            LOGES("connect failed, fd: %d, name: %s, sleep and connect \n",
                    socket->fd, socket->ipc_name);

            sleep(1);
        }
    } while ((ret < 0) && (++time_cnt < timeout_s));

    if (time_cnt < timeout_s) {
        return 0;
    } else {
        return -1;
    }
}

void hy_ipc_client_destroy(hy_ipc_socket_context_s **context_pp)
{
    LOGT("handle: %p, *handle: %p \n", context_pp, *context_pp);
    HY_ASSERT_VAL_RET(!context_pp || !*context_pp);

    hy_ipc_socket_context_s *context = *context_pp;
    hy_ipc_socket_s *socket = context->socket;

    close(socket->fd);

    LOGI("ipc socket client destroy, handle: %p, ipc_name: %s, name: %s, fd: %d \n",
            context->socket, socket->ipc_name, socket->name, socket->fd);
    hy_ipc_socket_socket_destroy(&socket);
}

hy_s32_t hy_ipc_client_create(hy_ipc_socket_context_s *context, const char *name)
{
    LOGT("handle: %p, name: %s \n", context, name);
    HY_ASSERT_VAL_RET_VAL(!context, -1);

    HyIpcSocketSaveConfig_s *save_config = &context->save_config;

    do {
        context->socket = hy_ipc_socket_socket_create(save_config->ipc_name, name);
        if (!context->socket) {
            LOGE("socket create failed \n");
            break;
        }

        LOGI("ipc socket client create \n");
        return 0;
    } while (0);

    return -1;
}
