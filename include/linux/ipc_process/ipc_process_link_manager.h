/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_link_manager.h
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
#ifndef __LIBHY_UTILS_INCLUDE_IPC_PROCESS_LINK_MANAGER_H_
#define __LIBHY_UTILS_INCLUDE_IPC_PROCESS_LINK_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_ipc_process.h"
#include "hy_list.h"

typedef void (*ipc_process_link_manager_accept_cb_t)(void);

typedef struct {
    struct hy_list_head                     list;

    void                                    *ipc_socket_handle;
    void                                    *accept_thread_handle;
    ipc_process_link_manager_accept_cb_t    accept_cb;

    hy_s32_t                                exit_flag:2;
    hy_s32_t                                reserved;
} ipc_process_link_manager_s;

void *ipc_process_link_manager_create(const char *ipc_name);
void ipc_process_link_manager_destroy(ipc_process_link_manager_s **link_manager);

#ifdef __cplusplus
}
#endif

#endif
