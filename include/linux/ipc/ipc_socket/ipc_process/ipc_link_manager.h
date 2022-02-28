/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link_manager.h
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
#ifndef __LIBHY_UTILS_INCLUDE_IPC_LINK_MANAGER_H_
#define __LIBHY_UTILS_INCLUDE_IPC_LINK_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ipc_link.h"

typedef void (*ipc_link_manager_accept_cb_t)(void *handle, void *args);

typedef struct {
    ipc_link_s                          *link;

    ipc_link_manager_accept_cb_t        accept_cb;
    void                                *args;
    void                                *link_manager_thread_h;

    struct hy_list_head                 list;
    pthread_mutex_t                     mutex;
} ipc_link_manager_s;

void *ipc_link_manager_create(const char *name, const char *tag,
        ipc_link_manager_accept_cb_t accpet, void *args);
void ipc_link_manager_destroy(ipc_link_manager_s **handle);

struct hy_list_head *ipc_link_manager_get_list(void *handle);
void ipc_link_manager_put_list(void *handle);

#ifdef __cplusplus
}
#endif

#endif

