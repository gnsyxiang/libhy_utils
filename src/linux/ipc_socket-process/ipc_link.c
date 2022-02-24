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

#include "ipc_link.h"

hy_s32_t ipc_link_wait_accept(ipc_link_s *ipc_link,
        HyIpcSocketAcceptCb_t accept_cb, void *args)
{
    LOGT("ipc_link: %p, args: %p \n", ipc_link, args);
    HY_ASSERT_RET_VAL(!ipc_link, -1);

    void *ipc_socket_handle = ipc_link->ipc_socket_handle;

    if (!ipc_socket_handle) {
        LOGE("ipc_socket_handle is NULL \n");
        return -1;
    }

    return HyIpcSocketAccept(ipc_link->ipc_socket_handle, accept_cb, args);
}

hy_s32_t ipc_link_connect(ipc_link_s *ipc_link, hy_u32_t timeout_s)
{
    LOGT("ipc_link: %p, timeout_s: %d \n", ipc_link, timeout_s);
    HY_ASSERT_RET_VAL(!ipc_link, -1);

    void *ipc_socket_handle = ipc_link->ipc_socket_handle;

    if (!ipc_socket_handle) {
        LOGE("ipc_socket_handle is NULL \n");
        return -1;
    }

    if (0 != HyIpcSocketConnect(ipc_socket_handle, timeout_s)) {
        LOGE("HyIpcSocketConnect failed \n");
        return -1;
    } else {
        return 0;
    }
}

hy_s32_t ipc_link_write(ipc_link_s *ipc_link, const void *buf, hy_u32_t len)
{
    LOGT("ipc_link: %p, buf: %p, len: %d \n", ipc_link, buf, len);
    HY_ASSERT(ipc_link);
    HY_ASSERT(buf);

    HyIpcSocketConnectState_e connect_state;
    hy_s32_t ret = 0;

    HyIpcSocketGetInfo(ipc_link->ipc_socket_handle,
            HY_IPC_SOCKET_INFO_CONNECT_STATE, &connect_state);

    if (connect_state == HY_IPC_SOCKET_CONNECT_STATE_CONNECT) {
        ret = HyIpcSocketWrite(ipc_link->ipc_socket_handle, &len, sizeof(hy_u32_t));
        if (ret == sizeof(hy_u32_t)) {
            ret = HyIpcSocketWrite(ipc_link->ipc_socket_handle, buf, len);
        }
    }

    if (ret != (hy_s32_t)len) {
        LOGE("ipc link write failed \n");
        return -1;
    } else {
        return ret;
    }
}

void ipc_link_destroy(ipc_link_s **ipc_link_pp)
{
    LOGT("&ipc_link: %p, ipc_link: %p \n", ipc_link_pp, *ipc_link_pp);
    HY_ASSERT_RET(!ipc_link_pp || !*ipc_link_pp);

    ipc_link_s *ipc_link = *ipc_link_pp;

    HyIpcSocketDestroy(&ipc_link->ipc_socket_handle);

    LOGI("ipc link destroy, ipc_link: %p \n", ipc_link);
    HY_MEM_FREE_PP(ipc_link_pp);
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
