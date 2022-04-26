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

#include "hy_hal/hy_list.h"

typedef struct {
    struct hy_list_head             entry;

    void                            *ipc_link_h;
} ipc_link_manager_list_s;

typedef void (*ipc_link_manager_accept_cb_t)(void *ipc_link_h, void *args);

typedef struct {
    ipc_link_manager_accept_cb_t    accept_cb;
    void                            *args;
} ipc_link_manager_save_config_s;

typedef struct {
    ipc_link_manager_save_config_s  save_config;

    void                            *ipc_link_h;
} ipc_link_manager_config_s;

void *ipc_link_manager_create(ipc_link_manager_config_s *ipc_link_manager_c);
void ipc_link_manager_destroy(void **ipc_link_manager_h);

struct hy_list_head *ipc_link_manager_list_get(void *ipc_link_manager_h);
void ipc_link_manager_list_put(void *ipc_link_manager_h);

#define ipc_link_manager_create_m(_accept_cb, _accept_cb_args, _ipc_link_h) \
    ({                                                                      \
        ipc_link_manager_config_s ipc_link_manager_c;                       \
        HY_MEMSET(&ipc_link_manager_c, sizeof(ipc_link_manager_c));         \
        ipc_link_manager_c.save_config.accept_cb    = _accept_cb;           \
        ipc_link_manager_c.save_config.args         = _accept_cb_args;      \
        ipc_link_manager_c.ipc_link_h               = _ipc_link_h;          \
        ipc_link_manager_create(&ipc_link_manager_c);                       \
     })

#ifdef __cplusplus
}
#endif

#endif

