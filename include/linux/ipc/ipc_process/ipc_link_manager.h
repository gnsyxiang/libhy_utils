/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link_manager.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 09:57
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 09:57
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_LINK_MANAGER_H_
#define __LIBHY_UTILS_INCLUDE_IPC_LINK_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ipc_link.h"

typedef void (*ipc_link_manager_accept_cb_t)(void *ipc_link_h, void *args);

typedef struct {
    ipc_link_manager_accept_cb_t    accept_cb;
    void                            *args;
} ipc_link_manager_save_config_s;

typedef struct {
    ipc_link_manager_save_config_s  save_config;

    const char                      *ipc_name;
    const char                      *tag;
} ipc_link_manager_config_s;

void *ipc_link_manager_create(ipc_link_manager_config_s *config);
void ipc_link_manager_destroy(void **ipc_link_manager_h);

#ifdef __cplusplus
}
#endif

#endif

