/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 15:54
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 15:54
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_file.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_log.h"

#include "hy_ipc_socket.h"
#include "ipc_socket_private.h"
#include "ipc_socket_client.h"
#include "ipc_socket_server.h"

hy_s32_t HyIpcSocketConnect(void *handle, hy_u32_t timeout_s)
{
    LOGT("handle: %p, timeout_s: %d \n", handle, timeout_s);
    HY_ASSERT_RET_VAL(!handle, -1);

    return ipc_socket_client_connect(handle, timeout_s);
}

hy_s32_t HyIpcSocketAccept(void *handle,
        HyIpcSocketAcceptCb_t accept_cb, void *args)
{
    LOGT("handle: %p, accept_cb: %p, args: %p \n", handle, accept_cb, args);
    HY_ASSERT_RET_VAL(!handle || !accept_cb, -1);

    return ipc_socket_server_accept(handle, accept_cb, args);
}

void HyIpcSocketGetInfo(void *handle, HyIpcSocketInfo_e info, void *data)
{
    LOGT("handle: %p, info: %d, data: %p \n", handle, info, data);
    HY_ASSERT_RET(!handle || !data);

    hy_ipc_socket_s *socket = handle;

    switch (info) {
        case HY_IPC_SOCKET_INFO_FD:
            *(hy_s32_t *) data = socket->fd;
            break;
        case HY_IPC_SOCKET_INFO_IPC_NAME:
            HY_MEMCPY(data, socket->ipc_name, HY_IPC_SOCKET_NAME_LEN_MAX);
            break;
        case HY_IPC_SOCKET_INFO_TYPE:
            *(HyIpcSocketType_e *)data = socket->type;
            break;
        default:
            break;
    }
}

hy_s32_t HyIpcSocketRead(void *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    return HyFileRead(((hy_ipc_socket_s *)handle)->fd, buf, len);
}

hy_s32_t HyIpcSocketWrite(void *handle, const void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    return HyFileWriteN(((hy_ipc_socket_s *)handle)->fd, buf, len);
}

void HyIpcSocketDestroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    hy_ipc_socket_s *socket = *handle;

    if (HY_IPC_SOCKET_TYPE_SERVER == socket->type) {
        return ipc_socket_server_destroy(handle);
    } else {
        return ipc_socket_client_destroy(handle);
    }
}

void *HyIpcSocketCreate(HyIpcSocketConfig_s *config)
{
    LOGT("ipc socket config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    if (HY_IPC_SOCKET_TYPE_SERVER == config->type) {
        return ipc_socket_server_create(config->ipc_name, config->type);
    } else {
        return ipc_socket_client_create(config->ipc_name, config->type);
    }
}
