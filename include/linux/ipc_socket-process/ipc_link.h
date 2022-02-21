/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/02 2022 16:36
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/02 2022      create the file
 * 
 *     last modified: 21/02 2022 16:36
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_LINK_H_
#define __LIBHY_UTILS_INCLUDE_IPC_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <unistd.h>

#include "hy_list.h"
#include "hy_ipc_socket_process.h"

typedef enum {
    IPC_LINK_TYPE_CLIENT,
    IPC_LINK_TYPE_SERVER,

    IPC_LINK_TYPE_MAX,
} ipc_link_type_e;

typedef struct {
    struct hy_list_head     list;
    void                    *ipc_socket_handle;

    pid_t                   pid;
    char                    tag[HY_IPC_SOCKET_PROCESS_IPC_NAME_LEN_MAX / 2];
    const char              *ipc_name;

    hy_u32_t                use_cnt;

    ipc_link_type_e         link_type:2;
    hy_s32_t                is_connect:1;
    hy_s32_t                reserved;
} ipc_link_s;

#ifdef __cplusplus
}
#endif

#endif

