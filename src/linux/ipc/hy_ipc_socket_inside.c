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
    LOGT("handle: %p, *handle: %p, \n", socket_pp, *socket_pp);
    HY_ASSERT_RET(!socket_pp || !*socket_pp);

    hy_ipc_socket_s *socket = *socket_pp;

    if (0 != pthread_mutex_destroy(&socket->mutex)) {
        LOGES("pthread_mutex_destroy failed \n");
        return;
    }

    LOGI("ipc socket scoket destroy, handle: %p \n", socket);
    HY_MEM_FREE_PP(socket_pp);
}

hy_ipc_socket_s *hy_ipc_socket_socket_create(const char *ipc_name,
        const char *name)
{
    LOGT("ipc_name: %s, name: %s \n", ipc_name, name);
    HY_ASSERT_RET_VAL(!ipc_name, NULL);

    hy_ipc_socket_s *ipc_socket = NULL;

    do {
        ipc_socket = HY_MEM_MALLOC_BREAK(hy_ipc_socket_s *, sizeof(*ipc_socket));

        ipc_socket->fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (ipc_socket->fd < 0) {
            LOGES("socket failed \n");
            break;
        }

        if (0 != pthread_mutex_init(&ipc_socket->mutex, NULL)) {
            LOGE("pthread_mutex_init failed \n");
            break;
        }

        ipc_socket->ipc_name = ipc_name;
        HY_STRNCPY(ipc_socket->name, HY_IPC_SOCKET_NAME_LEN_MAX, name, HY_STRLEN(name));

        LOGI("ipc socket socket create, handle: %p, ipc_name: %s, name: %s, fd: %d \n",
                ipc_socket, ipc_socket->ipc_name, ipc_socket->name, ipc_socket->fd);
        return ipc_socket;
    } while (0);

    hy_ipc_socket_socket_destroy(&ipc_socket);
    return NULL;
}
