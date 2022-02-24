/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_server.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    24/02 2022 15:41
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        24/02 2022      create the file
 * 
 *     last modified: 24/02 2022 15:41
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "ipc_link_server.h"
#include "ipc_process_server.h"

typedef struct {
    HyIpcProcessSaveConfig_s    save_config;

    void                        *ipc_link_handle;
} _ipc_process_server_context_s;

void ipc_process_server_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_process_server_context_s *context = *handle;

    ipc_link_server_destroy(&context->ipc_link_handle);

    LOGI("ipc process server destroy, context: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *ipc_process_server_create(HyIpcProcessConfig_s *config)
{
    LOGT("config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    _ipc_process_server_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_process_server_context_s *,
                sizeof(*context));

        HyIpcProcessSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        context->ipc_link_handle = ipc_link_server_create(config->ipc_name,
                config->tag);
        if (!context->ipc_link_handle) {
            LOGE("ipc_link_server_create failed \n");
            break;
        }

        LOGI("ipc process server create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc process server failed \n");
    ipc_process_server_destroy((void **)&context);
    return NULL;
}
