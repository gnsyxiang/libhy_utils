/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/02 2022 19:17
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/02 2022      create the file
 * 
 *     last modified: 21/02 2022 19:17
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "hy_ipc_socket.h"
#include "ipc_link.h"

void ipc_link_destroy(ipc_link_s **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    ipc_link_s *link = *handle;

    HyIpcSocketDestroy(&link->ipc_socket_handle);

    LOGI("ipc link destroy, link: %p \n", link);
    HY_MEM_FREE_PP(handle);
}

ipc_link_s *ipc_link_create(const char *name, const char *tag,
        ipc_link_type_e type, void *ipc_socket_handle)
{
    LOGT("name: %s, tag: %s, type: %d \n", name, tag, type);
    // HY_ASSERT_RET_VAL(!name || !tag, NULL);

    ipc_link_s *link = NULL;
    HyIpcSocketType_e ipc_socket_type[HY_IPC_SOCKET_TYPE_MAX] = {
        HY_IPC_SOCKET_TYPE_CLIENT, HY_IPC_SOCKET_TYPE_SERVER
    };

    do {
        link = HY_MEM_MALLOC_BREAK(ipc_link_s *, sizeof(*link));

        if (ipc_socket_handle) {
            link->ipc_socket_handle = ipc_socket_handle;
        } else {
            link->pid = getpid();
            HY_STRNCPY(link->tag, HY_IPC_SOCKET_PROCESS_IPC_NAME_LEN_MAX / 2,
                    tag, HY_STRLEN(tag));

            link->ipc_socket_handle = HyIpcSocketCreate_m(name,
                    ipc_socket_type[type]);
            if (!link->ipc_socket_handle) {
                LOGE("HyIpcSocketCreate_m failed \n");
                break;
            }
        }

        LOGI("ipc link create, link: %p \n", link);
        return link;
    } while (0);

    LOGI("ipc link create failed \n");
    ipc_link_destroy(&link);
    return NULL;
}
