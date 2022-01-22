/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_client.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    22/01 2022 14:54
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        22/01 2022      create the file
 * 
 *     last modified: 22/01 2022 14:54
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "ipc_process_client.h"
#include "ipc_process_link.h"

typedef struct {
    ipc_process_link_s                      *link;
} _ipc_process_client_s;

void ipc_process_client_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_process_client_s *client = *handle;

    ipc_process_link_destroy((void **)&client->link);

    LOGI("ipc process client destroy, client: %p \n", client);
    HY_MEM_FREE_PP(handle);
}

void *ipc_process_client_create(const char *ipc_name)
{
    LOGT("ipc process client ipc_name: %s \n", ipc_name);
    HY_ASSERT_RET_VAL(!ipc_name, NULL);

    _ipc_process_client_s *client = NULL;

    do {
        client = HY_MEM_MALLOC_BREAK(_ipc_process_client_s *, sizeof(*client));

        client->link = ipc_process_link_create(ipc_name,
                IPC_PROCESS_LINK_TYPE_CLIENT);
        if (!client->link) {
            LOGE("ipc_process_link_create failed \n");
            break;
        }

        if (0 != ipc_process_link_connect(client->link)) {
            LOGE("ipc_process_link_connect failed \n");
            break;
        }

        LOGI("ipc process client create, client: %p \n", client);
        return client;
    } while (0);

    LOGE("ipc process client create failed \n");
    ipc_process_client_destroy((void **)&client);
    return NULL;
}
