/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_client.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 16:22
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 16:22
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_PROCESS_CLIENT_H_
#define __LIBHY_UTILS_INCLUDE_IPC_PROCESS_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_ipc_process.h"

void *ipc_process_client_create(HyIpcProcessConfig_s *ipc_process_config);
void ipc_process_client_destroy(void **ipc_process_client_h);

hy_s32_t ipc_process_client_data_sync(void *ipc_process_client_h,
        HyIpcProcessMsgId_e id, void *send, hy_u32_t send_len,
        void *recv, hy_u32_t recv_len);

#ifdef __cplusplus
}
#endif

#endif

