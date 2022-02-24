/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link_client.h
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
#ifndef __LIBHY_UTILS_INCLUDE_IPC_LINK_CLIENT_H_
#define __LIBHY_UTILS_INCLUDE_IPC_LINK_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ipc_link.h"

typedef struct {
    ipc_link_s  *link;
} ipc_link_client_s;

ipc_link_client_s *ipc_link_client_create(const char *name,
        const char *tag, hy_u32_t timeout_s);
void ipc_link_client_destroy(ipc_link_client_s **handle);

#ifdef __cplusplus
}
#endif

#endif
