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

#include "hy_hal/hy_type.h"

#define IPC_LINK_IPC_NAME_LEN_MAX   (32)

typedef enum {
    IPC_LINK_TYPE_CLIENT,
    IPC_LINK_TYPE_SERVER,

    IPC_LINK_TYPE_MAX,
} ipc_link_type_e;

typedef void (*ipc_link_accept_cb_t)(void *ipc_socket_h, void *args);

typedef struct {
    hy_s32_t                fd;
    const char              *tag;
} ipc_link_info_s;

typedef struct {
    void                    *ipc_socket_h; // 当有该参数时，直接赋值，无需创建

    const char              *ipc_name;
    const char              *tag;

    ipc_link_type_e         type:2;
    hy_s32_t                reserved;
} ipc_link_config_s;

void *ipc_link_create(ipc_link_config_s *config);
void ipc_link_destroy(void **ipc_link_h);

hy_s32_t ipc_link_write(void *ipc_link_h, void *buf, hy_u32_t len);
hy_s32_t ipc_link_read(void *ipc_link_h, void **buf, hy_u32_t *len);

void ipc_link_info_get(void *ipc_link_h, ipc_link_info_s **info);
void ipc_link_info_set(void *ipc_link_h, ipc_link_info_s *info);

hy_s32_t ipc_link_connect(void *ipc_link_h, hy_u32_t timeout_s);
hy_s32_t ipc_link_wait_accept(void *ipc_link_h,
        ipc_link_accept_cb_t accept_cb, void *args);

void ipc_link_dump(void *ipc_link_h);

#ifdef __cplusplus
}
#endif

#endif

