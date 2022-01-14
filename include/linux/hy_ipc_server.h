/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_server.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    14/01 2022 10:10
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        14/01 2022      create the file
 * 
 *     last modified: 14/01 2022 10:10
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_IPC_SERVER_H_
#define __LIBHY_UTILS_INCLUDE_HY_IPC_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

#define HY_IPC_SERVER_NAME_LEN_MAX (32)

/**
 * @brief 服务器的通信类型
 */
typedef enum {
    HY_IPC_SERVER_COMMUNICATION_SINGLE,         ///< 一对一通信
    HY_IPC_SERVER_COMMUNICATION_MULTI,          ///< 一对多通信

    HY_IPC_SERVER_COMMUNICATION_MAX,
} HyIpcServerCommunicationType_e;

/**
 * @brief 描述连接状态
 */
typedef enum {
    HY_IPC_SERVER_CONNECT_STATUS_CONNECT,       ///< 连接
    HY_IPC_SERVER_CONNECT_STATUS_DISCONNECT,    ///< 断开

    HY_IPC_SERVER_CONNECT_STATUS_MAX,
} HyIpcServerConnectStatus_e;

/**
 * @brief ipc相关信息
 */
typedef struct {
    hy_u64_t    pid;                            ///< 进程pid
    const char  *name;                          ///< ipc名字
} HyIpcInfo_s;

/**
 * @brief 客户端连接上服务器回调
 *
 * @param ipc_server 服务器ipc相关信息
 * @param ipc_client 客户端ipc相关信息
 * @param connect_status 连接状态
 *
 * @return 成功返回0，失败返回-1
 */
typedef hy_s32_t (*HyIpcServerAcceptClientCb_t)(HyIpcInfo_s *ipc_server,
        HyIpcInfo_s *ipc_client, HyIpcServerConnectStatus_e connect_status);

/**
 * @brief 服务端接收到数据后的回调函数
 *
 * @param ipc_server_handle 服务端ipc句柄
 * @param ipc_client_handle 客户端ipc句柄
 * @param args 传递的参数
 *
 * @return 成功返回0，失败返回-1
 */
typedef hy_s32_t (*HyIpcServerFuncCb_t)(void *ipc_server_handle,
        void *ipc_client_handle, void *args);

/**
 * @brief 服务端回调函数集合
 */
typedef struct {
    hy_s32_t            func_id;                ///< 函数id
    HyIpcServerFuncCb_t func_cb;                ///< 函数id对应的处理函数
} HyIpcServerFunc_s;

typedef struct {
    char                            name[HY_IPC_SERVER_NAME_LEN_MAX];
    HyIpcServerAcceptClientCb_t     accept_client_cb;
} HyIpcServerSaveConfig_t;

typedef struct {
    HyIpcServerSaveConfig_t         save_config;
    HyIpcServerFunc_s               *func;
    HyIpcServerCommunicationType_e  communication_type;
} HyIpcServerConfig_s;

void *HyIpcServerCreate(HyIpcServerConfig_s *config);

#ifdef __cplusplus
}
#endif

#endif
