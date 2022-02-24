/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link_server.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/02 2022 16:41
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/02 2022      create the file
 * 
 *     last modified: 21/02 2022 16:41
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_LINK_SERVER_H_
#define __LIBHY_UTILS_INCLUDE_IPC_LINK_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ipc_link.h"

typedef struct {
    ipc_link_s              *link;

    void                    *accept_thread_handle;

    struct hy_list_head     list;
    pthread_mutex_t         mutex;
} ipc_link_server_s;

void *ipc_link_server_create(const char *name, const char *tag);
void ipc_link_server_destroy(ipc_link_server_s **handle);

#ifdef __cplusplus
}
#endif

#endif

