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

#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_assert.h"

#include "hy_ipc_socket_inside.h"

void hy_ipc_socket_socket_destroy(hy_ipc_socket_s **socket_pp)
{
    LOGT("&socket: %p, socket: %p, \n", socket_pp, *socket_pp);
    HY_ASSERT_RET(!socket_pp || !*socket_pp);

    hy_ipc_socket_s *socket = *socket_pp;

    close(socket->fd);

    LOGI("ipc socket scoket destroy, socket: %p, ipc_name: %s, fd: %d, type: %d \n",
            socket, socket->ipc_name, socket->fd, socket->type);
    HY_MEM_FREE_PP(socket_pp);
}

hy_ipc_socket_s *hy_ipc_socket_socket_create(const char *ipc_name,
        HyIpcSocketType_e type)
{
    LOGT("ipc_name: %s \n", ipc_name);
    HY_ASSERT_RET_VAL(!ipc_name, NULL);

    hy_ipc_socket_s *ipc_socket = NULL;

    do {
        ipc_socket = HY_MEM_MALLOC_BREAK(hy_ipc_socket_s *, sizeof(*ipc_socket));

        ipc_socket->fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (ipc_socket->fd < 0) {
            LOGES("socket failed \n");
            break;
        }

        ipc_socket->type = type;
        HY_MEMCPY(ipc_socket->ipc_name, ipc_name, HY_STRLEN(ipc_name));

        LOGI("ipc socket socket create, socket: %p, ipc_name: %s, fd: %d, type: %d \n",
                ipc_socket, ipc_socket->ipc_name, ipc_socket->fd, ipc_socket->type);
        return ipc_socket;
    } while (0);

    LOGE("ipc socket socket create failed \n");
    hy_ipc_socket_socket_destroy(&ipc_socket);
    return NULL;
}
