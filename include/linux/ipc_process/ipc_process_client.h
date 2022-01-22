/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_client.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    22/01 2022 14:55
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        22/01 2022      create the file
 * 
 *     last modified: 22/01 2022 14:55
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_PROCESS_CLIENT_H_
#define __LIBHY_UTILS_INCLUDE_IPC_PROCESS_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

void *ipc_process_client_create(const char *ipc_name);
void ipc_process_client_destroy(void **handle);

#ifdef __cplusplus
}
#endif

#endif

