/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_private.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 11:20
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 11:20
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_PROCESS_PRIVATE_H_
#define __LIBHY_UTILS_INCLUDE_IPC_PROCESS_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_ipc_process.h"
#include "hy_list.h"

typedef struct {
    HyIpcProcessSaveConfig_s    save_config;

    const char                  *tag;

    struct hy_list_head         callback_list;     // 用于保存回调函数
    struct hy_list_head         ipc_process_list;   // 用于保存客户端连接

    void                        *ipc_process_handle;
    union {
        struct {
            void                *server_handle;
        };
        struct {
            void                *client;
        };
    };

    hy_s32_t                    eixt_flag:1;
    hy_s32_t                    reserved;
} ipc_process_context_s;

#ifdef __cplusplus
}
#endif

#endif
