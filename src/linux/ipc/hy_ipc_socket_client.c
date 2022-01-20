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
    LOGT("context: %p, timeout_s: %d \n", context, timeout_s);
    HY_ASSERT_RET_VAL(!context, -1);

    hy_ipc_socket_s *socket = &context->socket;
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
        LOGI("connect ipc server, fd: %d \n", socket->fd);
        return 0;
    } else {
        LOGE("connect server failed \n");
        return -1;
    }
}

void hy_ipc_client_destroy(hy_ipc_socket_context_s **context_pp)
{
    LOGT("&context: %p, context: %p \n", context_pp, *context_pp);
    HY_ASSERT_RET(!context_pp || !*context_pp);

    LOGI("ipc socket client destroy \n");
}

hy_s32_t hy_ipc_client_create(hy_ipc_socket_context_s *context,
        const char *ipc_name)
{
    LOGT("context: %p, ipc_name: %s \n", context, ipc_name);
    HY_ASSERT_RET_VAL(!context, -1);

    do {
        LOGI("ipc socket client create \n");
        return 0;
    } while (0);

    LOGE("ipc socket client create failed \n");
    hy_ipc_client_destroy(&context);
    return -1;
}
