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

#include "ipc_link_client.h"

typedef struct {
    ipc_link_s  *link;
} _ipc_link_client_s;

hy_s32_t ipc_link_client_write_info(void *handle, pid_t pid)
{
    LOGT("handle: %p, pid: %d \n", handle, pid);
    HY_ASSERT_RET_VAL(!handle, -1);

    hy_u32_t total_len = sizeof(ipc_link_msg_s)
        + HY_IPC_PROCESS_IPC_NAME_LEN_MAX + sizeof(pid_t);
    hy_s32_t offset = 0;
    char *ipc_msg_buf = NULL;
    ipc_link_msg_s *ipc_msg = NULL;
    ipc_link_s *link = NULL;

    ipc_msg_buf = HY_MEM_MALLOC_RET_VAL(char *, total_len, -1);

    ipc_msg = (ipc_link_msg_s *)ipc_msg_buf;
    link = ((_ipc_link_client_s *)handle)->link;

    HY_MEMCPY(ipc_msg->buf + offset, link->tag, HY_STRLEN(link->tag));
    offset += HY_STRLEN(link->tag) + 1;

    HY_MEMCPY(ipc_msg->buf + offset, &pid, sizeof(pid_t));
    offset += sizeof(pid_t);

    ipc_msg->total_len  = sizeof(ipc_link_msg_s) + offset;
    ipc_msg->type       = IPC_LINK_MSG_TYPE_INFO;
    ipc_msg->thread_id  = pthread_self();
    ipc_msg->buf_len    = offset;

    return ipc_link_write(link, ipc_msg);
}

hy_s32_t ipc_link_client_is_connect(void *handle)
{
    LOGT("handle: %p \n", handle);
    HY_ASSERT_RET_VAL(!handle, 0);

    ipc_link_s *link = ((_ipc_link_client_s *)handle)->link;

    return (link->is_connect == HY_IPC_PROCESS_STATE_CONNECT) ? 1 : 0;
}

void ipc_link_client_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);
    _ipc_link_client_s *client_link = *handle;

    ipc_link_destroy(&client_link->link);

    LOGI("ipc link client destroy, client_link: %p \n", client_link);
    HY_MEM_FREE_PP(handle);
}

void *ipc_link_client_create(const char *name,
        const char *tag, hy_u32_t timeout_s)
{
    LOGT("name: %s, tag: %s, timeout_s: %d \n", name, tag, timeout_s);
    HY_ASSERT_RET_VAL(!name || !tag, NULL);

    _ipc_link_client_s *client_link = NULL;

    do {
        client_link = HY_MEM_MALLOC_BREAK(_ipc_link_client_s *,
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
    ipc_link_client_destroy((void **)&client_link);
    return NULL;
}
