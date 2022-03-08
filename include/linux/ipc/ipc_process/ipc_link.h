/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 08:53
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 08:53
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_LINK_H_
#define __LIBHY_UTILS_INCLUDE_IPC_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <unistd.h>

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_compile.h"

#define IPC_LINK_IPC_NAME_LEN_MAX   (32)

typedef enum {
    IPC_LINK_TYPE_CLIENT,
    IPC_LINK_TYPE_SERVER,

    IPC_LINK_TYPE_MAX,
} ipc_link_type_e;

typedef enum {
    IPC_LINK_CONNECT_STATE_DISCONNECT,
    IPC_LINK_CONNECT_STATE_CONNECT,

    IPC_LINK_CONNECT_STATE_MAX,
} ipc_link_connect_state_e;

typedef enum {
    IPC_LINK_MSG_TYPE_INFO,
    IPC_LINK_MSG_TYPE_ACK,
    IPC_LINK_MSG_TYPE_CB,
    IPC_LINK_MSG_TYPE_CB_ID,
} ipc_link_msg_type_e;

typedef void (*ipc_link_accept_cb_t)(void *ipc_socket_h, void *args);

typedef struct {
    hy_s32_t                fd;
    const char              *ipc_name;
    const char              *tag;
} ipc_link_info_s;

typedef struct {
    hy_u32_t    total_len;
    hy_s32_t    type;

    void        *ipc_link_h;

    hy_u32_t    buf_len;
    char        buf[];
} UNPACKED ipc_link_msg_s;

typedef struct {
    void                    *ipc_socket_h; // 当有该参数时，直接赋值，无需创建

    const char              *ipc_name;
    const char              *tag;

    ipc_link_type_e         type:2;
    hy_s32_t                reserved;
} ipc_link_config_s;

void *ipc_link_create(ipc_link_config_s *ipc_link_c);
void ipc_link_destroy(void **ipc_link_h);

hy_s32_t ipc_link_read(void *ipc_link_h, ipc_link_msg_s **ipc_link_msg);
hy_s32_t ipc_link_write(void *ipc_link_h, ipc_link_msg_s *ipc_link_msg, hy_s32_t flag);

hy_s32_t ipc_link_info_get(void *ipc_link_h, ipc_link_info_s *ipc_link_info);
hy_s32_t ipc_link_info_set(void *ipc_link_h, const char *tag);
hy_s32_t ipc_link_info_send(void *ipc_link_h, const char *tag, pid_t pid);

hy_s32_t ipc_link_connect(void *ipc_link_h, hy_u32_t timeout_s);
hy_s32_t ipc_link_wait_accept(void *ipc_link_h,
        ipc_link_accept_cb_t accept_cb, void *args);

hy_s32_t ipc_link_get_fd(void *ipc_link_h);

void ipc_link_dump(void *ipc_link_h);

#define ipc_link_create_m(_ipc_name, _tag, _type, _ipc_socket_h)    \
    ({                                                              \
        ipc_link_config_s _ipc_link_c;                              \
        HY_MEMSET(&_ipc_link_c, sizeof(_ipc_link_c));               \
        _ipc_link_c.ipc_name        = _ipc_name;                    \
        _ipc_link_c.tag             = _tag;                         \
        _ipc_link_c.type            = _type;                        \
        _ipc_link_c.ipc_socket_h    = _ipc_socket_h;                \
        ipc_link_create(&_ipc_link_c);                              \
     })

#ifdef __cplusplus
}
#endif

#endif

