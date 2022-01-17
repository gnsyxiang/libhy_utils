/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket_inside.c
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

#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_assert.h"

#include "hy_socket_inside.h"

void hy_socket_socket_destroy(hy_socket_s **socket_pp)
{
    LOGT("handle: %p, *handle: %p, \n", socket_pp, *socket_pp);
    HY_ASSERT_VAL_RET(!socket_pp || !*socket_pp);

    hy_socket_s *socket = *socket_pp;

    if (0 != pthread_mutex_destroy(&socket->mutex)) {
        LOGES("pthread_mutex_destroy failed \n");
        return;
    }

    LOGI("socket scoket destroy, handle: %p \n", socket);
    HY_MEM_FREE_PP(socket_pp);
}

hy_socket_s *hy_socket_socket_create(void)
{
    hy_socket_s *socket = NULL;
    do {
        socket = HY_MEM_MALLOC_BREAK(hy_socket_s *, sizeof(*socket));

        if (0 != pthread_mutex_init(&socket->mutex, NULL)) {
            LOGE("pthread_mutex_init failed \n");
            break;
        }

        socket->fd = -1;

        LOGI("socket socket create, handle: %p \n", socket);
        return socket;
    } while (0);

    hy_socket_socket_destroy(&socket);
    return NULL;
}
