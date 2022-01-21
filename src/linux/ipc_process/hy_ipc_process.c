/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process_server.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 11:12
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 11:12
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "hy_ipc_process.h"
#include "ipc_process_inside.h"
#include "ipc_process_server.h"

void HyIpcProcessDestroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    ipc_process_context_s *context = *handle;
    HyIpcProcessSaveConfig_s *save_config = &context->save_config;

    if (HY_IPC_PROCESS_TYPE_SERVER == save_config->type) {
        ipc_process_server_destroy((ipc_process_context_s **)handle);
    } else {
    }

    LOGI("ipc process destroy, context: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *HyIpcProcessCreate(HyIpcProcessConfig_s *config)
{
    LOGT("ipc process config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    ipc_process_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(ipc_process_context_s *, sizeof(*context));

        HyIpcProcessSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        if (HY_IPC_PROCESS_TYPE_SERVER == config->save_config.type) {
            ipc_process_server_create(context, config->ipc_name);
        } else {
        }

        LOGI("ipc process create, context: %p \n", context);
        return context;
    } while (0);

    LOGI("ipc process create failed \n");
    HyIpcProcessDestroy((void **)&context);
    return NULL;
}
