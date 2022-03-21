/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    02/03 2022 16:50
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        02/03 2022      create the file
 * 
 *     last modified: 02/03 2022 16:50
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_IPC_PROCESS_H_
#define __LIBHY_UTILS_INCLUDE_HY_IPC_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <unistd.h>

#include "hy_hal/hy_type.h"

// note:
// 1，该头文件需要用户提供，用来定义传输的数据（可以是结构体，也可以是json）
// 2，需要指定该头文件的搜索路径
#include "hy_ipc_process_id.h"

typedef enum {
    HY_IPC_PROCESS_TYPE_CLIENT,                         ///< 客户端
    HY_IPC_PROCESS_TYPE_SERVER,                         ///< 服务器

    HY_IPC_PROCESS_TYPE_MAX,
} HyIpcProcessType_e;

typedef enum {
    HY_IPC_PROCESS_CONNECT_STATE_DISCONNECT,
    HY_IPC_PROCESS_CONNECT_STATE_CONNECT,

    HY_IPC_PROCESS_CONNECT_STATE_MAX,
} HyIpcProcessConnectState_e;

typedef struct {
    pid_t       pid;
    const char  *ipc_name;
    const char  *tag;
} HyIpcProcessInfo_s;

typedef void (*HyIpcProcessStateChangeCb_t)(HyIpcProcessInfo_s *ipc_process_info,
        HyIpcProcessConnectState_e is_connect, void *args);

typedef hy_s32_t (*HyIpcProcessFuncCb_t)(void *recv, hy_u32_t recv_len,
        void *send, hy_u32_t send_len, void *args);

typedef struct {
    HyIpcProcessMsgId_e             id;
    HyIpcProcessFuncCb_t            func_cb;
} HyIpcProcessFunc_s;

typedef struct {
    HyIpcProcessStateChangeCb_t     state_change_cb;
    void                            *args;

    HyIpcProcessType_e              type:2;
    hy_s32_t                        reserved;
} HyIpcProcessSaveConfig_s;

typedef struct {
    HyIpcProcessSaveConfig_s        save_config;

    const char                      *ipc_name;
    const char                      *tag;

    HyIpcProcessFunc_s              *func;
    void                            *func_args;
    hy_u32_t                        func_cnt;

    // client
    hy_u32_t                        connect_timeout_s;
} HyIpcProcessConfig_s;

void *HyIpcProcessCreate(HyIpcProcessConfig_s *ipc_process_c);
void HyIpcProcessDestroy(void **ipc_process_h);

hy_s32_t HyIpcProcessDataSync(void *ipc_process_h, HyIpcProcessMsgId_e id,
        void *send, hy_u32_t send_len, void *recv, hy_u32_t recv_len);
hy_s32_t HyIpcProcessDataBroadcast(void *ipc_process_h, HyIpcProcessMsgId_e id,
        void *send, hy_u32_t send_len, void **recv, hy_u32_t *recv_len);

HyIpcProcessInfo_s HyIpcProcessInfoGet(HyIpcProcessType_e type);

#define HyIpcProcessDataSync_m(_ipc_process_h, _id, _send, _recv, _timeout) \
    HyIpcProcessDataSync(_ipc_process_h, _id, _send, sizeof(*_send), _recv, sizeof(*_recv));

#ifdef __cplusplus
}
#endif

#endif

