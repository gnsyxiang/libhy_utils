/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket.c
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
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_mem.h"

#include "hy_socket.h"
#include "hy_socket_inside.h"
#include "hy_socket_server.h"

hy_s32_t HySocketAccept(void *handle, HySocketAcceptCb_t accept_cb, void *args)
{
    LOGT("handle: %p, accept_cb: %p \n", handle, accept_cb);
    HY_ASSERT_VAL_RET_VAL(!handle || !accept_cb, -1);

    hy_socket_context_s *context = handle;

    return hy_server_accept(context, accept_cb, args);
}

static void _exec_socket_func(hy_socket_context_s *context,
        HySocketType_e type, hy_s32_t op)
{
    struct {
        const char *str;
        hy_s32_t (*socket_create_cb)(hy_socket_context_s *context);
        void (*socket_destroy_cb)(hy_socket_context_s **context_pp);
    } socket_create[HY_SOCKET_TYPE_MAX] = {
        {"client",  hy_client_create, hy_client_destroy},
        {"server",  hy_server_create, hy_server_destroy},
    };

    if (op) {
        if (0 != socket_create[type].socket_create_cb(context)) {
            LOGE("%s create failed \n", socket_create[type].str);
        }
    } else {
        socket_create[type].socket_destroy_cb(&context);
    }
}

void HySocketDestroy(void **handle)
{
    LOGT("handle: %p, *handle: %p \n", handle, *handle);
    HY_ASSERT_VAL_RET(!handle || !*handle);

    hy_socket_context_s *context = *handle;
    HySocketSaveConfig_s *save_config = &context->save_config;

    _exec_socket_func(context, save_config->type, 0);

    LOGI("socket destroy, handle: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *HySocketCreate(HySocketConfig_s *config)
{
    LOGT("config: %p \n", config);
    HY_ASSERT_VAL_RET_VAL(!config, NULL);

    hy_socket_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(hy_socket_context_s *, sizeof(*context));

        HySocketSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        _exec_socket_func(context, save_config->type, 1);

        LOGI("socket create, handle: %p \n", context);
        return context;
    } while (0);

    HySocketDestroy((void **)&context);
    return NULL;
}
