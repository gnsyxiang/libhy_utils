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

typedef void (*ipc_link_server_accept_cb)(void *handle, void *args);
typedef hy_s32_t (*ipc_link_server_detect_fd_cb)(void *args);
typedef hy_s32_t (*ipc_link_server_detect_fd_info_cb_t)(void *handle, void *args);

typedef struct {
    ipc_link_server_detect_fd_info_cb_t detect_fd_info_cb;
    void *args;
} ipc_link_manager_parse_cb_s;

typedef struct {
    ipc_link_s                  *link;

    ipc_link_server_accept_cb   accept;
    void                        *args;
    void                        *accept_thread_handle;

    struct hy_list_head         list;
    pthread_mutex_t             mutex;
} ipc_link_manager_s;

void *ipc_link_manager_create(const char *name, const char *tag,
        ipc_link_server_accept_cb accpet, void *args);
void ipc_link_manager_destroy(void **handle);

void ipc_link_server_set_fd(void *handle, fd_set *read_fs);
void ipc_link_server_detect_fd(void *handle, fd_set *read_fs,
        ipc_link_manager_parse_cb_s *parse_cb);

hy_s32_t ipc_link_manager_parse(ipc_link_s *ipc_link,
        ipc_link_manager_parse_cb_s *parse_cb);

hy_s32_t ipc_link_server_get_fd(void *handle);

struct hy_list_head *ipc_link_manager_get_list(void *handle);
void ipc_link_manager_put_list(void *handle);

#ifdef __cplusplus
}
#endif

#endif

