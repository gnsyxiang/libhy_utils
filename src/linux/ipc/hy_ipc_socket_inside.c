/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket_inside.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 11:21
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 11:21
 */
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_log.h"

#include "hy_ipc_socket_inside.h"

void hy_ipc_socket_socket_get_info(hy_ipc_socket_s *socket,
        HyIpcSocketInfo_e info, void *data)
{
    LOGT("socket: %p, info: %d, data: %p \n", socket, info, data);
    HY_ASSERT_RET(!socket || !data);

    switch (info) {
        case HY_IPC_SOCKET_INFO_FD:
            *(hy_s32_t *) data = socket->fd;
            break;
        case HY_IPC_SOCKET_INFO_IPC_NAME:
            if (HY_STRLEN(socket->ipc_name) < HY_IPC_SOCKET_NAME_LEN_MAX) {
                HY_MEMCPY(data, socket->ipc_name, HY_STRLEN(socket->ipc_name) + 1);
            } else {
                LOGW("the ipc name is too long \n");

                HY_MEMCPY(data, socket->ipc_name, HY_IPC_SOCKET_NAME_LEN_MAX);
            }
            break;
        case HY_IPC_SOCKET_INFO_TYPE:
            *(HyIpcSocketType_e *)data = socket->type;
            break;
        default:
            break;
    }
}

void hy_ipc_socket_socket_destroy(hy_ipc_socket_s **socket_pp)
{
    LOGT("&socket: %p, socket: %p, \n", socket_pp, *socket_pp);
    HY_ASSERT_RET(!socket_pp || !*socket_pp);

    hy_ipc_socket_s *socket = *socket_pp;

    LOGI("ipc socket scoket destroy, socket: %p, ipc_name: %s, type: %d \n",
            socket, socket->ipc_name, socket->type);
    HY_MEM_FREE_PP(socket_pp);
}

hy_ipc_socket_s *hy_ipc_socket_socket_create(const char *ipc_name,
        HyIpcSocketType_e type)
{
    LOGT("ipc_name: %s \n", ipc_name);
    HY_ASSERT_RET_VAL(!ipc_name, NULL);

    hy_ipc_socket_s *socket = NULL;

    do {
        socket = HY_MEM_MALLOC_BREAK(hy_ipc_socket_s *, sizeof(*socket));

        socket->type = type;
        HY_MEMCPY(socket->ipc_name, ipc_name, HY_STRLEN(ipc_name));

        LOGI("ipc socket socket create, socket: %p, ipc_name: %s, type: %d \n",
                socket, socket->ipc_name, socket->type);
        return socket;
    } while (0);

    LOGE("ipc socket socket create failed \n");
    hy_ipc_socket_socket_destroy(&socket);
    return NULL;
}
