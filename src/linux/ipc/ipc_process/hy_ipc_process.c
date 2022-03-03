/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 10:22
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 10:22
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"

#include "hy_ipc_process.h"
#include "ipc_process_server.h"
#include "ipc_process_client.h"

void HyIpcProcessDestroy(void **ipc_process_h)
{
    LOGT("&ipc_process_h: %p, ipc_process_h: %p \n",
            ipc_process_h, *ipc_process_h);
    HY_ASSERT_RET(!ipc_process_h || !*ipc_process_h);

    HyIpcProcessSaveConfig_s *save_config = *ipc_process_h;

    void (*_destroy_cb[HY_IPC_PROCESS_TYPE_MAX])(void **handle) = {
        ipc_process_client_destroy,
        ipc_process_server_destroy,
    };

    _destroy_cb[save_config->type](ipc_process_h);
}

void *HyIpcProcessCreate(HyIpcProcessConfig_s *ipc_process_c)
{
    LOGT("ipc_process_c: %p \n", ipc_process_c);
    HY_ASSERT_RET_VAL(!ipc_process_c, NULL);


    void *(*_create_cb[HY_IPC_PROCESS_TYPE_MAX])(
            HyIpcProcessConfig_s *config) = {
        ipc_process_client_create,
        ipc_process_server_create,
    };

    return _create_cb[ipc_process_c->save_config.type](ipc_process_c);
}

