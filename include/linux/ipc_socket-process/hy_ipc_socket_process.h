/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket_process.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    22/01 2022 16:57
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        22/01 2022      create the file
 * 
 *     last modified: 22/01 2022 16:57
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_IPC_SOCKET_PROCESS_H_
#define __LIBHY_UTILS_INCLUDE_HY_IPC_SOCKET_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <unistd.h>

// note:
// 1，该头文件需要用户提供，可以参考ipc_process_id.h
// 2，需要指定该头文件的搜索路径
#include "ipc_process_id.h"

#define HY_IPC_SOCKET_PROCESS_IPC_NAME_LEN_MAX (64)

/**
 * @brief ipc-process类型
 */
typedef enum {
    HY_IPC_SOCKET_PROCESS_TYPE_CLIENT,          ///< 客户端
    HY_IPC_SOCKET_PROCESS_TYPE_SERVER,          ///< 服务器

    HY_IPC_SOCKET_PROCESS_TYPE_MAX,
} HyIpcSocketProcessType_e;

/**
 * @brief 连接状态
 */
typedef enum {
    HY_IPC_SOCKET_PROCESS_STATE_CONNECT,        ///< 连上
    HY_IPC_SOCKET_PROCESS_STATE_DISCONNECT,     ///< 断开

    HY_IPC_SOCKET_PROCESS_STATE_MAX,
} HyIpcSocketProcessConnectState_e;

/**
 * @brief id回调函数
 *
 * @param server_handle 服务器句柄
 * @param client_handle 客户端句柄
 * @param args 上层传递参数
 *
 * @return 成功返回0，失败返回-1
 */
typedef hy_s32_t (*HyIpcSocketProcessCallbackCb_t)(void *server_handle,
        void *client_handle, void *args);

/**
 * @brief id回调结构体
 */
typedef struct {
    HyIpcSocketProcessId_e          id;             ///< id
    HyIpcSocketProcessCallbackCb_t  callback_cb;    ///< id回调函数
} HyIpcSocketProcessCallbackCb_s;

/**
 * @brief ipc-process相关信息
 */
typedef struct {
    pid_t           pid;            ///< 进程id
    const char      *ipc_name;      ///< ipc-process名字
    const char      *tag;           ///< ipc-process标记
} HyIpcSocketProcessInfo_s;

/**
 * @brief ipc-process状态回调函数
 *
 * @param handle 句柄
 * @param is_connect 连接或断开
 * @param args 上层参数
 */
typedef void (*HyIpcSocketProcessConnectChangeCb_t)(void *handle,
        HyIpcSocketProcessConnectState_e is_connect, void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    char                                    tag[HY_IPC_SOCKET_PROCESS_IPC_NAME_LEN_MAX / 2];    ///< ipc-process标记

    HyIpcSocketProcessConnectChangeCb_t     connect_change;                                     ///< ipc-process状态回调函数
    const void                              *args;                                              ///< 上层传递参数
    HyIpcSocketProcessType_e                type:2;                                             ///< ipc-process类型
    hy_s32_t                                reserved;                                           ///< 预留
} HyIpcSocketProcessSaveConfig_s;

typedef struct {
    HyIpcSocketProcessSaveConfig_s          save_config;    ///< 配置参数

    char                                    *ipc_name;      ///< ipc-process名字
    HyIpcSocketProcessCallbackCb_s          *callback;      ///< id回调结构体
    hy_u32_t                                callback_cnt;   ///< id回调个数
} HyIpcSocketProcessConfig_s;

/**
 * @brief 创建ipc-process模块
 *
 * @param config 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
void *HyIpcSocketProcessCreate(HyIpcSocketProcessConfig_s *config);

/**
 * @brief 销毁ipc-process模块
 *
 * @param handle 句柄的地址
 */
void HyIpcSocketProcessDestroy(void **handle);

/**
 * @brief 发送消息
 *
 * @param handle 句柄
 * @param msg 消息结构体
 * @param len 消息结构体长度
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyIpcSocketProcessSend(void *handle, void *msg, hy_u32_t len);

/**
 * @brief 广播消息
 *
 * @param handle 句柄
 * @param msg 消息结构体
 * @param len 消息结构体长度
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyIpcSocketProcessBroadcast(void *handle, void *msg, hy_u32_t len);

/**
 * @brief 获取相关信息
 *
 * @param handle 句柄
 * @param info ipc-process相关信息
 */
void HyIpcSocketProcessGetInfo(void *handle, HyIpcSocketProcessInfo_s *info);

#ifdef __cplusplus
}
#endif

#endif