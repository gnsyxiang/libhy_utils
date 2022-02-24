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

void ipc_link_client_destroy(ipc_link_client_s **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);
    ipc_link_client_s *client_link = *handle;

    ipc_link_destroy(&client_link->link);

    LOGI("ipc link client destroy, client_link: %p \n", client_link);
    HY_MEM_FREE_PP(handle);
}

void *ipc_link_client_create(const char *name,
        const char *tag, hy_u32_t timeout_s)
{
    LOGT("name: %s, tag: %s \n", name, tag);
    LOGT("name: %s, tag: %s, timeout_s: %d \n", name, tag, timeout_s);
    HY_ASSERT_RET_VAL(!name || !tag, NULL);

    ipc_link_client_s *client_link = NULL;

    do {
        client_link = HY_MEM_MALLOC_BREAK(ipc_link_client_s *,
                sizeof(*client_link));

        client_link->link = ipc_link_create(name, tag,
                IPC_LINK_TYPE_CLIENT, NULL);
        if (!client_link->link) {
            LOGE("ipc_link_create failed \n");
            break;
        }

        if (0 != ipc_link_connect(client_link->link, timeout_s)) {
            LOGE("ipc_link_connect failed \n");
            break;
        }

        LOGI("ipc link client create, link: %p \n", client_link);
        return client_link;
    } while (0);

    LOGE("ipc link client create failed \n");
    ipc_link_client_destroy(&client_link);
    return NULL;
}
