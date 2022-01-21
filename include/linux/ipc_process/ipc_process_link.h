/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_link.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 15:14
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 15:14
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_PROCESS_LINK_H_
#define __LIBHY_UTILS_INCLUDE_IPC_PROCESS_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_ipc_process.h"
#include "hy_list.h"

typedef enum {
    IPC_PROCESS_LINK_TYPE_SERVER,
    IPC_PROCESS_LINK_TYPE_CLIENT,

    IPC_PROCESS_LINK_TYPE_MAX,
} ipc_process_link_type_e;

typedef struct {
    struct hy_list_head         entry;

    void                        *ipc_socket_handle;
    char                        tag[HY_IPC_PROCESS_IPC_NAME_LEN_MAX / 2];
    hy_u32_t                    pid;

    ipc_process_link_type_e     link_type:2;
    hy_s32_t                    reserved;
} ipc_process_link_s;

void *ipc_process_link_create();

#ifdef __cplusplus
}
#endif

#endif
