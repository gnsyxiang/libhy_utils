/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_server.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 16:07
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 16:07
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_PROCESS_SERVER_H_
#define __LIBHY_UTILS_INCLUDE_IPC_PROCESS_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_list.h"
#include "hy_ipc_process.h"
#include "ipc_process_link.h"

typedef struct {
    void                                    *mutex_handle;
    struct hy_list_head                     list;

    ipc_process_link_s                      *link;
    void                                    *accept_thread_handle;
    ipc_process_link_accept_cb_t            accept_cb;

    hy_s32_t                                exit_flag:2;
    hy_s32_t                                reserved;
} ipc_process_server_s;

void *ipc_process_server_create(const char *ipc_name);
void ipc_process_server_destroy(void **handle);

#ifdef __cplusplus
}
#endif

#endif
