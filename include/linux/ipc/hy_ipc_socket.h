/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_socket.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 08:21
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 08:21
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_IPC_SOCKET_H_
#define __LIBHY_UTILS_INCLUDE_HY_IPC_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

#define HY_IPC_SOCKET_NAME_LEN_MAX  (32)

/**
 * @brief socket类型
 */
typedef enum {
    HY_IPC_SOCKET_TYPE_CLIENT,      ///< 客户端
    HY_IPC_SOCKET_TYPE_SERVER,      ///< 服务端

    HY_IPC_SOCKET_TYPE_MAX,
} HyIpcSocketType_e;

/**
 * @brief socket相关信息
 */
typedef enum {
    HY_IPC_SOCKET_INFO_FD,          ///< socket的fd

    HY_IPC_SOCKET_INFO_MAX,
} HyIpcSocketInfo_e;

/**
 * @brief 接收客户端回调
 *
 * @param fd 客户端fd
 * @param ipc_name ipc socket管道
 * @param name 服务器名字
 * @param args 上层传递参数
 */
typedef void (*HyIpcSocketAcceptCb_t)(hy_s32_t fd,
        const char *ipc_name, const char *name, void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    char                        ipc_name[HY_IPC_SOCKET_NAME_LEN_MAX];       ///< 服务器名字
    HyIpcSocketType_e           type:2;                                     ///< socket类型
    HyIpcSocketType_e           reserved;                                   ///< 预留
} HyIpcSocketSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyIpcSocketSaveConfig_s     save_config;                                ///< 配置参数

    char                        name[HY_IPC_SOCKET_NAME_LEN_MAX];           ///< 多个客户端的标识
} HyIpcSocketConfig_s;

/**
 * @brief 创建socket
 *
 * @param config 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
void *HyIpcSocketCreate(HyIpcSocketConfig_s *config);

/**
 * @brief 销毁socket
 *
 * @param handle 句柄的地址
 */
void HyIpcSocketDestroy(void **handle);

/**
 * @brief 读取数据
 *
 * @param handle 句柄
 * @param buf 数据地址
 * @param len 数据长度
 *
 * @return 成功返回读到的字节数，失败返回-1
 */
hy_s32_t HyIpcSocketRead(void *handle, void *buf, hy_u32_t len);

/**
 * @brief 写入数据
 *
 * @param handle 句柄
 * @param buf 数据地址
 * @param len 数据长度
 *
 * @return 成功返回写入的字节数，失败返回-1
 */
hy_s32_t HyIpcSocketWrite(void *handle, const void *buf, hy_u32_t len);

void HyIpcSocketGetInfo(void *handle);

/**
 * @brief 服务端等待客户端连接
 *
 * @param handle 句柄
 * @param accept_cb 接收客户端回调
 * @param args 上层传递的参数
 *
 * @return 错误返回-1，退出返回0
 */
hy_s32_t HyIpcSocketAccept(void *handle,
        HyIpcSocketAcceptCb_t accept_cb, void *args);

/**
 * @brief 客户端连接服务器
 *
 * @param handle 句柄
 * @param timeout_s 超时时间，0表示阻塞执行
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyIpcSocketConnect(void *handle, hy_u32_t timeout_s);


#ifdef __cplusplus
}
#endif

#endif
