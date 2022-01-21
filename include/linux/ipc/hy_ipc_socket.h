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

#define HY_IPC_SOCKET_NAME_LEN_MAX  (64)

/**
 * @brief ipc socket类型
 */
typedef enum {
    HY_IPC_SOCKET_TYPE_CLIENT,                  ///< 客户端
    HY_IPC_SOCKET_TYPE_SERVER,                  ///< 服务端

    HY_IPC_SOCKET_TYPE_MAX,
} HyIpcSocketType_e;

/**
 * @brief ipc socket相关信息
 */
typedef enum {
    HY_IPC_SOCKET_INFO_FD,                      ///< ipc socket的fd
    HY_IPC_SOCKET_INFO_IPC_NAME,                ///< ipc socket绑定的文件
    HY_IPC_SOCKET_INFO_TYPE,                    ///< ipc socket的类型

    HY_IPC_SOCKET_INFO_MAX,
} HyIpcSocketInfo_e;

/**
 * @brief 接收客户端回调
 *
 * @param handle 句柄
 * @param args 上层传递参数
 */
typedef void (*HyIpcSocketAcceptCb_t)(void *handle, void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    const char                  *ipc_name;      ///< 服务器名字
    HyIpcSocketType_e           type:2;         ///< ipc socket类型
    hy_s32_t                    reserved;       ///< 预留
} HyIpcSocketConfig_s;

/**
 * @brief 创建ipc socket
 *
 * @param config 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
void *HyIpcSocketCreate(HyIpcSocketConfig_s *config);

/**
 * @brief 销毁ipc socket
 *
 * @param handle 句柄的地址(二级指针)
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

/**
 * @brief 获取ipc socket相关信息
 *
 * @param handle 句柄
 * @param info socket相关信息
 * @param data 保存信息的地址
 *
 * @note 
 * 1, data是传出参数，需要上层开辟空间
 * 2，当info为HY_IPC_SOCKET_INFO_FD，data为hy_s32_t类型
 * 3, 当info为HY_IPC_SOCKET_INFO_IPC_NAME，data为数组类型，以便保存数据，
 *    数组长度必须等于或大于HY_IPC_SOCKET_NAME_LEN_MAX
 * 4，当info为HY_IPC_SOCKET_INFO_TYPE，data为HyIpcSocketType_e类型
 *
 * 特别注意获取名字时
 *     数组的长度一定要大于或等于HY_IPC_SOCKET_NAME_LEN_MAX，否则造成内存问题
 */
void HyIpcSocketGetInfo(void *handle, HyIpcSocketInfo_e info, void *data);

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

/**
 * @brief 创建ipc socket
 *
 * @param _ipc_name 服务器名字
 * @param _type socket类型，详见HyIpcSocketType_e
 *
 * @return 成功返回句柄，失败返回NULL
 */
#define HyIpcSocketCreate_m(_ipc_name, _type)                   \
    ({                                                          \
        HyIpcSocketConfig_s config;                             \
        HY_MEMSET(&config, sizeof(config));                     \
        config.type      = _type;                               \
        config.ipc_name  = _ipc_name;                           \
        HyIpcSocketCreate(&config);                             \
     })

#ifdef __cplusplus
}
#endif

#endif
