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

void HySocketDestroy(void **handle)
{
    LOGT("handle: %p, *handle: %p \n", handle, *handle);
    HY_ASSERT_VAL_RET(!handle || !*handle);

    hy_socket_context_s *context = *handle;
    HySocketSaveConfig_s *save_config = &context->save_config;

    if (save_config->type == HY_SOCKET_TYPE_SERVER) {
        hy_server_destroy(&context);
    } else {
    }

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

        if (save_config->type == HY_SOCKET_TYPE_SERVER) {
            if (0 != hy_server_create(context)) {
                LOGE("server create failed \n");
                break;
            }
        } else {
        }

        LOGI("socket create, handle: %p \n", context);
        return context;
    } while (0);

    HySocketDestroy((void **)&context);
    return NULL;
}
