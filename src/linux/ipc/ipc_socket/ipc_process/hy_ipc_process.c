/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process.c
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

#include "hy_ipc_process.h"
#include "ipc_link_client.h"
#include "ipc_link_server.h"

typedef struct {
    HyIpcProcessSaveConfig_s        save_config;

    union {
        // client
        struct {
            ipc_link_client_s       *client_link_handle;
        };

        // server
        struct {
            ipc_link_server_s       *server_link_handle;
        };
    };
} _ipc_process_context_s;

void HyIpcProcessDestroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_process_context_s *context = *handle;
    HyIpcProcessSaveConfig_s *save_config = &context->save_config;

    if (save_config->type == HY_IPC_PROCESS_TYPE_SERVER) {
        ipc_link_server_destroy(&context->server_link_handle);
    } else {
        ipc_link_client_destroy(&context->client_link_handle);
    }

    LOGI("ipc process destroy, context: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *HyIpcProcessCreate(HyIpcProcessConfig_s *config)
{
    LOGT("config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    _ipc_process_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_process_context_s *, sizeof(*context));

        HyIpcProcessSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        if (save_config->type == HY_IPC_PROCESS_TYPE_SERVER) {
            context->server_link_handle
                = ipc_link_server_create(config->ipc_name, config->tag);
            if (!context->server_link_handle) {
                LOGE("ipc_link_server_create failed \n");
                break;
            }
        } else {
            context->client_link_handle
                = ipc_link_client_create(config->ipc_name, config->tag, config->timeout_s);
            if (!context->client_link_handle) {
                LOGE("ipc_link_client_create failed \n");
                break;
            }
        }

        LOGI("ipc process create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc process create failed \n");
    HyIpcProcessDestroy((void **)&context);
    return NULL;
}
