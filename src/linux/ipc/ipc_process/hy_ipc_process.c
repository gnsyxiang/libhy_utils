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

#include <hy_os/hy_assert.h>

#include "hy_ipc_process.h"

#include "ipc_process_server.h"
#include "ipc_process_client.h"

hy_s32_t HyIpcProcessDataSync(void *ipc_process_h, HyIpcProcessMsgId_e id,
        void *send, hy_u32_t send_len, void *recv, hy_u32_t recv_len)
{
    LOGT("ipc_process_h: %p, id: %d \n", ipc_process_h, id);
    HY_ASSERT_RET_VAL(!ipc_process_h, -1);

    HyIpcProcessSaveConfig_s *save_config = ipc_process_h;

    hy_s32_t (*_data_sync[HY_IPC_PROCESS_TYPE_MAX])(void *ipc_process_h,
            HyIpcProcessMsgId_e id, void *send, hy_u32_t send_len,
            void *recv, hy_u32_t recv_len) = {
        ipc_process_client_data_sync,
        NULL,
    };

    if (_data_sync[save_config->type]) {
        return _data_sync[save_config->type](ipc_process_h, id,
                send, send_len, recv, recv_len);
    } else {
        return -1;
    }
}

hy_s32_t HyIpcProcessDataBroadcast(void *ipc_process_h, HyIpcProcessMsgId_e id,
        void *send, hy_u32_t send_len, void **recv, hy_u32_t *recv_len)
{
    return 0;
}

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

