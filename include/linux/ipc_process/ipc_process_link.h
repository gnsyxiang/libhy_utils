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
    IPC_PROCESS_LINK_TYPE_NO_SOCKET,
    IPC_PROCESS_LINK_TYPE_CLIENT,
    IPC_PROCESS_LINK_TYPE_SERVER,

    IPC_PROCESS_LINK_TYPE_MAX,
} ipc_process_link_type_e;

typedef void (*ipc_process_link_accept_cb_t)(void *handle, void *args);

typedef struct {
    struct hy_list_head         list;

    void                        *socket;
    char                        tag[HY_IPC_PROCESS_IPC_NAME_LEN_MAX / 2];

    ipc_process_link_type_e     link_type:2;
    hy_s32_t                    reserved;
} ipc_process_link_s;

ipc_process_link_s *ipc_process_link_create(const char *ipc_name,
        ipc_process_link_type_e type);
ipc_process_link_s *ipc_process_link_create_2(void *link);
void ipc_process_link_destroy(void **handle);

hy_s32_t ipc_process_link_accept(void *handle,
        ipc_process_link_accept_cb_t accept_cb, void *args);

hy_s32_t ipc_process_link_connect(void *handle);

#ifdef __cplusplus
}
#endif

#endif
