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
#include <pthread.h>

#include "hy_list.h"
#include "hy_ipc_socket.h"
#include "hy_ipc_process.h"

typedef enum {
    IPC_LINK_TYPE_CLIENT,
    IPC_LINK_TYPE_SERVER,

    IPC_LINK_TYPE_MAX,
} ipc_link_type_e;

typedef enum {
    IPC_LINK_MSG_TYPE_ACK,
    IPC_LINK_MSG_TYPE_INFO,
    IPC_LINK_MSG_TYPE_CB,
    IPC_LINK_MSG_TYPE_CB_ID,

    IPC_LINK_MSG_TYPE_MAX,
} ipc_link_msg_type_e;

typedef struct {
    struct hy_list_head     entry;

    void                    *ipc_socket_handle;

    pid_t                   pid;
    char                    tag[HY_IPC_PROCESS_IPC_NAME_LEN_MAX / 2];

    hy_u32_t                use_cnt;

    ipc_link_type_e         link_type:2;
    hy_s32_t                is_connect:2;
    hy_s32_t                reserved;
} ipc_link_s;

typedef struct {
    hy_s32_t                total_len;
    hy_s32_t                type;

    hy_s32_t                pfd1;
    pthread_t               thread_id;

    hy_s32_t                buf_len;
    char                    buf[];
} ipc_link_msg_s;

typedef struct {
    struct hy_list_head     entry;

    void                    *ipc_link;
    ipc_link_msg_s          *ipc_msg;
} ipc_link_msg_usr_s;

typedef struct {
    struct hy_list_head     entry;
    hy_u32_t                *id;
    hy_u32_t                id_cnt;
} ipc_link_id_s;

ipc_link_s *ipc_link_create(const char *name, const char *tag,
        ipc_link_type_e type, void *ipc_socket_handle);
void ipc_link_destroy(ipc_link_s **ipc_link);

hy_s32_t ipc_link_read(ipc_link_s *ipc_link, ipc_link_msg_s **ipc_msg);
hy_s32_t ipc_link_write(ipc_link_s *ipc_link, ipc_link_msg_s *ipc_msg);

hy_s32_t ipc_link_write_info(ipc_link_s *ipc_link, const char *tag, pid_t pid);
hy_s32_t ipc_link_write_cb(ipc_link_s *ipc_link, hy_u32_t *id, hy_u32_t id_cnt);

void ipc_link_set_info(ipc_link_s *ipc_link, const char *tag, pid_t pid);
void ipc_link_get_info(ipc_link_s *ipc_link, HyIpcProcessInfo_s *ipc_process_info);

hy_s32_t ipc_link_connect(ipc_link_s *ipc_link, hy_u32_t timeout_s);
hy_s32_t ipc_link_wait_accept(ipc_link_s *ipc_link,
        HyIpcSocketAcceptCb_t accept_cb, void *args);

hy_s32_t ipc_link_get_fd(ipc_link_s *ipc_link);
void ipc_link_dump(ipc_link_s *ipc_link);

ipc_link_msg_usr_s *ipc_link_msg_usr_create(void *ipc_link,
        ipc_link_msg_s *ipc_msg);
void ipc_link_msg_usr_destroy(ipc_link_msg_usr_s *ipc_msg_usr);

#ifdef __cplusplus
}
#endif

#endif

