/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_server.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 16:48
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 16:48
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_PROCESS_SERVER_H_
#define __LIBHY_UTILS_INCLUDE_IPC_PROCESS_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ipc_process_private.h"

hy_s32_t ipc_process_server_create(ipc_process_context_s *context,
        const char *ipc_name);
void ipc_process_server_destroy(ipc_process_context_s **context);

#ifdef __cplusplus
}
#endif

#endif

