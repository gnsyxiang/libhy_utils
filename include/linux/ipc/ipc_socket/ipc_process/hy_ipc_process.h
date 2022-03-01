/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process.h
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
#ifndef __LIBHY_UTILS_INCLUDE_HY_IPC_PROCESS_H_
#define __LIBHY_UTILS_INCLUDE_HY_IPC_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <unistd.h>

// note:
// 1，该头文件需要用户提供，可以参考ipc_process_id.h
// 2，需要指定该头文件的搜索路径
#include "ipc_process_id.h"

#define HY_IPC_PROCESS_IPC_NAME_LEN_MAX                 (64)

/**
 * @brief ipc_process类型
 */
typedef enum {
    HY_IPC_PROCESS_TYPE_CLIENT,                         ///< 客户端
    HY_IPC_PROCESS_TYPE_SERVER,                         ///< 服务器

    HY_IPC_PROCESS_TYPE_MAX,
} HyIpcProcessType_e;

/**
 * @brief ipc_process连接状态
 */
typedef enum {
    HY_IPC_PROCESS_STATE_DISCONNECT,                    ///< 断开
    HY_IPC_PROCESS_STATE_CONNECT,                       ///< 连上

    HY_IPC_PROCESS_STATE_MAX,
} HyIpcProcessConnectState_e;

/**
 * @brief id回调函数
 *
 * @param server_handle 服务器句柄
 * @param client_handle 客户端句柄
 * @param args 上层传递参数
 *
 * @return 成功返回0，失败返回-1
 */
typedef hy_s32_t (*HyIpcProcessCallbackCb_t)(void *buf, hy_u32_t len, void *args);

/**
 * @brief id回调结构体
 */
typedef struct {
    HyIpcProcessMsgId_e         id;                     ///< id
    HyIpcProcessCallbackCb_t    callback_cb;            ///< id回调函数
    void                        *args;                  ///< 上层传递参数
} HyIpcProcessCallbackCb_s;

/**
 * @brief ipc_process相关信息
 */
typedef struct {
    pid_t           pid;                                ///< 进程id
    const char      *ipc_name;                          ///< ipc_process名字
    const char      *tag;                               ///< ipc_process标记
} HyIpcProcessInfo_s;

/**
 * @brief ipc_process状态回调函数
 *
 * @param handle 句柄
 * @param is_connect 连接或断开
 * @param args 上层参数
 */
typedef void (*HyIpcProcessConnectChangeCb_t)(
        HyIpcProcessInfo_s *ipc_process_info,
        HyIpcProcessConnectState_e is_connect, void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    HyIpcProcessConnectChangeCb_t   connect_change;     ///< ipc_process状态回调函数
    void                            *args;              ///< 上层传递参数
    HyIpcProcessType_e              type:2;             ///< ipc_process类型
    hy_s32_t                        reserved;           ///< 预留
} HyIpcProcessSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyIpcProcessSaveConfig_s        save_config;        ///< 配置参数

    const char                      *ipc_name;          ///< ipc_process名字
    const char                      *tag;               ///< ipc_process标记

    HyIpcProcessCallbackCb_s        *callback;          ///< id回调结构体
    hy_u32_t                        callback_cnt;       ///< id回调个数

    // client
    hy_u32_t                        timeout_s;          ///< 连接超时时间，0为阻塞
} HyIpcProcessConfig_s;

/**
 * @brief 创建ipc_process模块
 *
 * @param config 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
void *HyIpcProcessCreate(HyIpcProcessConfig_s *config);

/**
 * @brief 销毁ipc_process模块
 *
 * @param handle 句柄的地址
 */
void HyIpcProcessDestroy(void **handle);

/**
 * @brief 发送消息
 *
 * @param handle 句柄
 * @param msg 消息结构体
 * @param len 消息结构体长度
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyIpcProcessSend(void *handle, void *msg, hy_u32_t len);

/**
 * @brief 广播消息
 *
 * @param handle 句柄
 * @param msg 消息结构体
 * @param len 消息结构体长度
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyIpcProcessBroadcast(void *handle, void *msg, hy_u32_t len);

/**
 * @brief 获取相关信息
 *
 * @param handle 句柄
 * @param info ipc_process相关信息
 */
void HyIpcProcessGetInfo(void *handle, HyIpcProcessInfo_s *info);

#ifdef __cplusplus
}
#endif

#endif
