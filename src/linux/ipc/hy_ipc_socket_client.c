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
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "ipc_socket_private.h"
#include "hy_ipc_socket_client.h"

typedef struct {
    hy_ipc_socket_s socket;
} _ipc_socket_client_context_t;

hy_s32_t hy_ipc_client_connect(void *handle, hy_u32_t timeout_s)
{
    LOGT("handle: %p, timeout_s: %d \n", handle, timeout_s);
    HY_ASSERT_RET_VAL(!handle, -1);

    _ipc_socket_client_context_t *context = handle;
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

void hy_ipc_client_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_socket_client_context_t *context = *handle;
    hy_ipc_socket_s *ipc_socket = &context->socket;

    close(ipc_socket->fd);

    LOGI("ipc socket client destroy, context: %p, fd: %d \n",
            context, ipc_socket->fd);
    HY_MEM_FREE_PP(handle);
}

void *hy_ipc_client_create(const char *ipc_name, HyIpcSocketType_e type)
{
    LOGT("ipc_name: %s, type: %d \n", ipc_name, type);
    HY_ASSERT_RET_VAL(!ipc_name, NULL);

    _ipc_socket_client_context_t *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_socket_client_context_t *, sizeof(*context));

        context->socket.type = type;
        HY_MEMCPY(context->socket.ipc_name, ipc_name, HY_STRLEN(ipc_name));

        hy_ipc_socket_s *ipc_socket = &context->socket;

        ipc_socket->fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (ipc_socket->fd < 0) {
            LOGES("socket failed \n");
            break;
        }

        LOGI("ipc socket client create, context: %p, fd: %d \n",
                context, ipc_socket->fd);
        return context;
    } while (0);

    LOGE("ipc socket client create failed \n");
    hy_ipc_client_destroy((void **)&context);
    return NULL;
}
