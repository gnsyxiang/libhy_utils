/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_client.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    24/02 2022 16:01
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        24/02 2022      create the file
 * 
 *     last modified: 24/02 2022 16:01
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "ipc_link_client.h"
#include "ipc_process_client.h"

typedef struct {
    HyIpcProcessSaveConfig_s    save_config;

    pid_t                       pid;
    void                        *ipc_link_handle;
} _ipc_process_client_context_s;

void ipc_process_client_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_process_client_context_s *context = *handle;

    ipc_link_client_destroy(&context->ipc_link_handle);

    LOGI("ipc process client destroy, context: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *ipc_process_client_create(HyIpcProcessConfig_s *config)
{
    LOGT("config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    _ipc_process_client_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_process_client_context_s *,
                sizeof(*context));

        context->pid = getpid();

        HyIpcProcessSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        context->ipc_link_handle = ipc_link_client_create(config->ipc_name,
                config->tag, config->timeout_s);
        if (!context->ipc_link_handle) {
            LOGE("ipc_link_client_create failed \n");
            break;
        }

        if (0 != ipc_link_client_write_info(context->ipc_link_handle,
                    context->pid)) {
            LOGE("ipc_link_client_write_info failed \n");
            break;
        }

        LOGI("ipc process client create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc process client failed \n");
    ipc_process_client_destroy((void **)&context);
    return NULL;
}
