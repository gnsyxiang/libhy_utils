/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_socket_private.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    22/01 2022 11:13
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        22/01 2022      create the file
 * 
 *     last modified: 22/01 2022 11:13
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "ipc_socket_private.h"

void ipc_socket_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    LOGI("socket: %p \n", *handle);
    HY_MEM_FREE_PP(handle);
}

void *ipc_socket_create(const char *ipc_name, HyIpcSocketType_e type)
{
    LOGT("ipc_name: %s, type: %d \n", ipc_name, type);
    HY_ASSERT_RET_VAL(!ipc_name, NULL);

    hy_ipc_socket_s *socket = NULL;

    do {
        socket = HY_MEM_MALLOC_BREAK(hy_ipc_socket_s *, sizeof(*socket));

        socket->type = type;
        HY_MEMCPY(socket->ipc_name, ipc_name, HY_STRLEN(ipc_name));

        LOGI("ipc socket create, socket: %p \n", socket);
        return socket;
    } while (0);

    LOGE("ipc socket create failed \n");
    ipc_socket_destroy((void **)&socket);
    return NULL;
}
