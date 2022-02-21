/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link_client.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/02 2022 13:54
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/02 2022      create the file
 * 
 *     last modified: 17/02 2022 13:54
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "hy_ipc_socket.h"
#include "ipc_link_client.h"

void ipc_link_client_destroy(ipc_link_s **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);
    ipc_link_s *link = *handle;

    HyIpcSocketDestroy(&link->ipc_socket_handle);

    LOGI("ipc link client destroy \n");
    HY_MEM_FREE_PP(handle);
}

void *ipc_link_client_create(const char *name, const char *tag)
{
    LOGT("name: %s, tag: %s \n", name, tag);
    HY_ASSERT_RET_VAL(!name || !tag, NULL);

    ipc_link_s *link = NULL;

    do {
        link = HY_MEM_MALLOC_BREAK(ipc_link_s *, sizeof(*link));

        link->ipc_socket_handle = HyIpcSocketCreate_m(name, HY_IPC_SOCKET_TYPE_CLIENT);
        if (!link->ipc_socket_handle) {
            LOGE("HyIpcSocketCreate_m failed \n");
            break;
        }

        if (0 != HyIpcSocketConnect(link->ipc_socket_handle, -1)) {
            LOGE("HyIpcSocketConnect failed \n");
            break;
        }

        LOGI("ipc link client create, link: %p \n", link);
        return link;
    } while (0);

    LOGE("ipc link client create failed \n");
    ipc_link_client_destroy(&link);
    return NULL;
}
