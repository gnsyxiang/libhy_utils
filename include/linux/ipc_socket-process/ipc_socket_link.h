/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_socket_link.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    22/01 2022 17:24
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        22/01 2022      create the file
 * 
 *     last modified: 22/01 2022 17:24
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_SOCKET_LINK_H_
#define __LIBHY_UTILS_INCLUDE_IPC_SOCKET_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <unistd.h>

#include "hy_list.h"
#include "hy_ipc_socket_process.h"

typedef enum {
    IPC_SOCKET_LINK_TYPE_CLIENT,
    IPC_SOCKET_LINK_TYPE_SERVER,

    IPC_SOCKET_LINK_TYPE_MAX,
} ipc_socket_link_type_e;

typedef struct {
    struct hy_list_head     list;
    void                    *ipc_socket;

    pid_t                   pid;
    char                    tag[HY_IPC_SOCKET_PROCESS_IPC_NAME_LEN_MAX / 2];
    const char              *ipc_name;

    hy_u32_t                use_cnt;

    ipc_socket_link_type_e  link_type:2;
    hy_s32_t                is_connect:1;
    hy_s32_t                reserved;
} ipc_socket_link_s;

void *ipc_socket_link_create(void);
void ipc_socket_link_destroy(ipc_socket_link_s **handle);

hy_s32_t ipc_socket_link_read(ipc_socket_link_s *link);

#ifdef __cplusplus
}
#endif

#endif
