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
    LOGT("handle: %p, timeout_s: %d \n", handle, timeout_s);
    HY_ASSERT_RET_VAL(!handle, -1);

    hy_ipc_socket_context_s *context
        = HY_CONTAINER_OF(handle, hy_ipc_socket_context_s, socket);

    return hy_ipc_client_connect(context, timeout_s);
}

hy_s32_t HyIpcSocketAccept(void *handle,
        HyIpcSocketAcceptCb_t accept_cb, void *args)
{
    LOGT("handle: %p, accept_cb: %p \n", handle, accept_cb);
    HY_ASSERT_RET_VAL(!handle || !accept_cb, -1);

    hy_ipc_socket_context_s *context
        = HY_CONTAINER_OF(handle, hy_ipc_socket_context_s, socket);

    return hy_ipc_server_accept(context, accept_cb, args);
}

hy_s32_t HyIpcSocketRead(void *handle, void *buf, hy_u32_t len)
{
    LOGT("handle: %p, buf: %p, len: %d \n", handle, buf, len);
    HY_ASSERT_RET_VAL(!handle || !buf, -1);

    hy_ipc_socket_s *socket = handle;

    return HyFileRead(socket->fd, buf, len);
}

hy_s32_t HyIpcSocketWrite(void *handle, const void *buf, hy_u32_t len)
{
    LOGT("handle: %p, buf: %p, len: %d \n", handle, buf, len);
    HY_ASSERT_RET_VAL(!handle || !buf, -1);

    hy_ipc_socket_s *socket = handle;

    return HyFileWriteN(socket->fd, buf, len);
}

static void _exec_ipc_socket_func(hy_ipc_socket_context_s *context,
        HyIpcSocketType_e type, hy_s32_t op)
{
    struct {
        const char *str;
        hy_s32_t (*create_cb)(hy_ipc_socket_context_s *);
        void (*destroy_cb)(hy_ipc_socket_context_s **);
    } socket_create[HY_IPC_SOCKET_TYPE_MAX] = {
        {"client",      hy_ipc_client_create,   hy_ipc_client_destroy},
        {"server",      hy_ipc_server_create,   hy_ipc_server_destroy},
    };

    if (op) {
        if (0 != socket_create[type].create_cb(context)) {
            LOGE("ipc socket %s create failed \n", socket_create[type].str);
        }
    } else {
        socket_create[type].destroy_cb(&context);
    }
}

void HyIpcSocketDestroy(void **handle)
{
    LOGT("handle: %p, *handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    hy_ipc_socket_context_s *context
        = HY_CONTAINER_OF(*handle, hy_ipc_socket_context_s, socket);
    HyIpcSocketSaveConfig_s *save_config = &context->save_config;

    _exec_ipc_socket_func(context, save_config->type, 0);

    LOGI("ipc socket destroy, context: %p, socket: %p \n",
            context, context->socket);
    HY_MEM_FREE_PP(handle);
}

void *HyIpcSocketCreate(HyIpcSocketConfig_s *config)
{
    LOGT("ipc socket config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    hy_ipc_socket_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(hy_ipc_socket_context_s *, sizeof(*context));

        HyIpcSocketSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        _exec_ipc_socket_func(context, save_config->type, 1);

        LOGI("ipc socket create, context: %p, socket: %p \n",
                context, context->socket);
        return &context->socket;
    } while (0);

    HyIpcSocketDestroy((void **)&context->socket);
    return NULL;
}
