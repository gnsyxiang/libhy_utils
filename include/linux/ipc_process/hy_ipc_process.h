/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 11:04
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 11:04
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_IPC_PROCESS_H_
#define __LIBHY_UTILS_INCLUDE_HY_IPC_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

#define HY_IPC_PROCESS_IPC_NAME_LEN_MAX (64)

typedef enum {
    HY_IPC_PROCESS_TYPE_CLIENT,
    HY_IPC_PROCESS_TYPE_SERVER,

    HY_IPC_PROCESS_TYPE_MAX,
} HyIpcProcessType_e;

typedef enum {
    HY_IPC_PROCESS_ID_TEST,

    HY_IPC_PROCESS_ID_MAX,
} HyIpcProcessId_e;

typedef hy_s32_t (*HyIpcProcessCallbackCb_t)(void *server_handle,
        void *client_handle, void *args);

typedef struct {
    HyIpcProcessId_e            id;
    HyIpcProcessCallbackCb_t    callback_cb;
} HyIpcProcessCallbackCb_s;

typedef void (*HyIpcProcessConnectChangeCb_t)(void *server_handle,
        void *client_handle, hy_s32_t is_connect, void *args);

typedef struct {
    char                            tag[HY_IPC_PROCESS_IPC_NAME_LEN_MAX / 2];

    HyIpcProcessConnectChangeCb_t   connect_change;
    const void                      *args;
    HyIpcProcessType_e              type:2;
    hy_s32_t                        reserved;       ///< 预留
} HyIpcProcessSaveConfig_s;

typedef struct {
    HyIpcProcessSaveConfig_s    save_config;

    char                        ipc_name[HY_IPC_PROCESS_IPC_NAME_LEN_MAX / 2];
    HyIpcProcessCallbackCb_s    *callback;
    hy_u32_t                    callback_cnt;
} HyIpcProcessConfig_s;

void *HyIpcProcessCreate(HyIpcProcessConfig_s *config);
void HyIpcProcessDestroy(void **handle);

void HyIpcProcessSend(void *handle);    // 需要回复确认消息
void HyIpcProcessPost(void *handle);    // 广播发送

#ifdef __cplusplus
}
#endif

#endif
