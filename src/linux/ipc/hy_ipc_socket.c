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
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_file.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_mem.h"

#include "hy_ipc_socket.h"
#include "hy_ipc_socket_inside.h"
#include "hy_ipc_socket_client.h"
#include "hy_ipc_socket_server.h"

hy_s32_t HyIpcSocketConnect(void *handle, hy_u32_t timeout_s)
{
    LOGT("context: %p, timeout_s: %d \n", handle, timeout_s);
    HY_ASSERT_RET_VAL(!handle, -1);

    return hy_ipc_client_connect((hy_ipc_socket_context_s *)handle, timeout_s);
}

hy_s32_t HyIpcSocketAccept(void *handle,
        HyIpcSocketAcceptCb_t accept_cb, void *args)
{
    LOGT("context: %p, accept_cb: %p, args: %p \n", handle, accept_cb, args);
    HY_ASSERT_RET_VAL(!handle || !accept_cb, -1);

    return hy_ipc_server_accept((hy_ipc_socket_context_s *)handle, accept_cb, args);
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

static hy_s32_t _exec_ipc_socket_func(hy_ipc_socket_context_s *context,
        const char *ipc_name, HyIpcSocketType_e type, hy_s32_t op)
{
    struct {
        hy_s32_t (*create_cb)(hy_ipc_socket_context_s *, const char *);
        void (*destroy_cb)(hy_ipc_socket_context_s **);
    } socket_create[HY_IPC_SOCKET_TYPE_MAX] = {
        {hy_ipc_client_create,   hy_ipc_client_destroy},
        {hy_ipc_server_create,   hy_ipc_server_destroy},
    };

    if (op) {
        return socket_create[type].create_cb(context, ipc_name);
    } else {
        socket_create[type].destroy_cb(&context);
    }

    return 0;
}

void HyIpcSocketDestroy(void **handle)
{
    LOGT("&context: %p, context: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    hy_ipc_socket_context_s *context = *handle;
    hy_ipc_socket_s *socket = &context->socket;

    close(socket->fd);

    _exec_ipc_socket_func(context, NULL, socket->type, 0);

    LOGI("ipc socket destroy, context: %p, fd: %d \n", context, socket->fd);
    HY_MEM_FREE_PP(handle);
}

void *HyIpcSocketCreate(HyIpcSocketConfig_s *config)
{
    LOGT("ipc socket config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    hy_ipc_socket_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(hy_ipc_socket_context_s *,
                sizeof(*context));

        hy_ipc_socket_s *ipc_socket = &context->socket;

        ipc_socket->fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (ipc_socket->fd < 0) {
            LOGES("socket failed \n");
            break;
        }

        ipc_socket->type = config->type;
        HY_MEMCPY(ipc_socket->ipc_name,
                config->ipc_name, HY_STRLEN(config->ipc_name));

        if (0 != _exec_ipc_socket_func(context,
                    config->ipc_name, config->type, 1)) {
            LOGE("call server/client func failed \n");
            break;
        }

        LOGI("ipc socket create, context: %p, fd: %d \n",
                context, ipc_socket->fd);
        return context;
    } while (0);

    LOGE("ipc socket create failed \n");
    HyIpcSocketDestroy((void **)&context);
    return NULL;
}
