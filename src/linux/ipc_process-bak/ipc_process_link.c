/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_link.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 20:28
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 20:28
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_log.h"

#include "hy_ipc_socket.h"

#include "ipc_process_link.h"

hy_s32_t ipc_process_link_connect(void *handle)
{
    LOGT("handle: %p \n", handle);
    HY_ASSERT_RET_VAL(!handle, -1);

    ipc_process_link_s *link = handle;

    return HyIpcSocketConnect(link->socket, 10);
}

hy_s32_t ipc_process_link_accept(void *handle,
        ipc_process_link_accept_cb_t accept_cb, void *args)
{
    LOGT("handle: %p, accept_cb: %p \n", handle, accept_cb);
    HY_ASSERT_RET_VAL(!handle || !accept_cb, -1);

    ipc_process_link_s *link = handle;

    return HyIpcSocketAccept(link->socket, accept_cb, args);
}

void ipc_process_link_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    ipc_process_link_s *link = *handle;

    HyIpcSocketDestroy(&link->socket);

    LOGI("ipc process link destroy, link: %p \n", link);
    HY_MEM_FREE_PP(handle);
}

ipc_process_link_s *ipc_process_link_create(const char *ipc_name,
        ipc_process_link_type_e type)
{
    LOGT("ipc_name: %s, type: %d \n", ipc_name, type);
    HY_ASSERT_RET_VAL(!ipc_name, NULL);

    ipc_process_link_s *link = NULL;

    do {
        link = HY_MEM_MALLOC_BREAK(ipc_process_link_s *, sizeof(*link));

        link->link_type = type;

        HyIpcSocketType_e socket_type[] = {
            HY_IPC_SOCKET_TYPE_NO_SOCKET,
            HY_IPC_SOCKET_TYPE_CLIENT,
            HY_IPC_SOCKET_TYPE_SERVER
        };
        link->socket = HyIpcSocketCreate_m(ipc_name, socket_type[type]);
        if (!link->socket) {
            LOGE("HyIpcSocketCreate_m failed \n");
            break;
        }

        LOGI("ipc process link create, link: %p \n", link);
        return link;
    } while (0);

    LOGE("ipc process link create failed \n");
    ipc_process_link_destroy((void **)&link);
    return NULL;
}

ipc_process_link_s *ipc_process_link_create_2(void *socket)
{
    LOGT("socket: %p \n", socket);
    HY_ASSERT_RET_VAL(!socket, NULL);

    ipc_process_link_s *link = NULL;

    do {
        link = HY_MEM_MALLOC_BREAK(ipc_process_link_s *, sizeof(*link));

        link->socket = socket;
        link->link_type = IPC_PROCESS_LINK_TYPE_CLIENT;

        LOGI("ipc process link create, link: %p \n", link);
        return link;
    } while (0);

    LOGE("ipc process link create failed \n");
    ipc_process_link_destroy((void **)&link);
    return NULL;
}