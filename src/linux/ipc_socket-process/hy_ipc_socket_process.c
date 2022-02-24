/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket_process.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/02 2022 11:36
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/02 2022      create the file
 * 
 *     last modified: 17/02 2022 11:36
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "hy_ipc_socket_process.h"
#include "ipc_link_client.h"
#include "ipc_link_server.h"

typedef struct {
    HyIpcSocketProcessSaveConfig_s  save_config;

    union {
        // client
        struct {
            ipc_link_client_s       *link_handle;
        };

        // server
        struct {
            ipc_link_server_s       *server_link_handle;
        };
    };
} _ipc_socket_process_context_s;

void HyIpcSocketProcessDestroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_socket_process_context_s *context = *handle;
    HyIpcSocketProcessSaveConfig_s *save_config = &context->save_config;

    if (save_config->type == HY_IPC_SOCKET_PROCESS_TYPE_SERVER) {
        ipc_link_server_destroy(&context->server_link_handle);
    } else {
        ipc_link_client_destroy(&context->link_handle);
    }

    LOGI("ipc socket process destroy \n");
    HY_MEM_FREE_PP(handle);
}

void *HyIpcSocketProcessCreate(HyIpcSocketProcessConfig_s *config)
{
    LOGT("ipc socket process config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    _ipc_socket_process_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_socket_process_context_s *, sizeof(*context));

        HyIpcSocketProcessSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        if (save_config->type == HY_IPC_SOCKET_PROCESS_TYPE_SERVER) {
            context->link_handle = ipc_link_server_create(config->ipc_name,
                    config->tag);
        } else {
            context->link_handle = ipc_link_client_create(config->ipc_name,
                    config->tag, config->timeout_s);
        }
        if (!context->link_handle) {
            LOGE("ipc link create failed \n");
            break;
        }

        LOGI("ipc socket process create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc socket process create failed \n");
    HyIpcSocketProcessDestroy((void **)&context);
    return NULL;
}
