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

#include "ipc_process_client.h"
#include "ipc_process_server.h"
#include "hy_ipc_process.h"

void HyIpcProcessDestroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    HyIpcProcessSaveConfig_s *save_config = *handle;

    void (*_destroy_cb[HY_IPC_PROCESS_TYPE_MAX])(void **handle) = {
        ipc_process_client_destroy,
        ipc_process_server_destroy,
    };

    _destroy_cb[save_config->type](handle);
}

void *HyIpcProcessCreate(HyIpcProcessConfig_s *config)
{
    LOGT("config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    void *(*_create_cb[HY_IPC_PROCESS_TYPE_MAX])(
            HyIpcProcessConfig_s *config) = {
        ipc_process_client_create,
        ipc_process_server_create,
    };

    return _create_cb[config->save_config.type](config);
}
