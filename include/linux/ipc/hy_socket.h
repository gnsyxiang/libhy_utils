/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket.h
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
#ifndef __LIBHY_UTILS_INCLUDE_HY_SOCKET_H_
#define __LIBHY_UTILS_INCLUDE_HY_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

#define HY_SOCKET_NAME_LEN_MAX  (32)

/**
 * @brief socket类型
 */
typedef enum {
    HY_SOCKET_TYPE_CLIENT,      ///< 客户端
    HY_SOCKET_TYPE_SERVER,      ///< 服务端

    HY_SOCKET_TYPE_MAX,
} HySocketType_e;

/**
 * @brief socket相关信息
 */
typedef enum {
    HY_SOCKET_INFO_FD,          ///< socket的fd

    HY_SOCKET_INFO_MAX,
} HySocketInfo_e;

/**
 * @brief 接收客户端回调
 *
 * @param fd 客户端fd
 * @param args 上层传递参数
 */
typedef void (*HySocketAcceptCb_t)(hy_s32_t fd, void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    char            name[HY_SOCKET_NAME_LEN_MAX];   ///< socket名字
    HySocketType_e  type:2;                         ///< socket类型
    HySocketType_e  reserved;
} HySocketSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HySocketSaveConfig_s save_config;               ///< 配置参数
} HySocketConfig_s;

/**
 * @brief 创建socket
 *
 * @param config 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
void *HySocketCreate(HySocketConfig_s *config);

/**
 * @brief 销毁socket
 *
 * @param handle 句柄的地址
 */
void HySocketDestroy(void **handle);

/**
 * @brief 服务端等待客户端连接
 *
 * @param handle 句柄
 * @param accept_cb 接收客户端回调
 * @param args 上层传递的参数
 *
 * @return 错误返回-1，退出返回0
 */
hy_s32_t HySocketAccept(void *handle, HySocketAcceptCb_t accept_cb, void *args);

/**
 * @brief 客户端连接服务器
 *
 * @param handle 句柄
 * @param timeout_s 超时时间，0表示阻塞执行
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HySocketConnect(void *handle, hy_u32_t timeout_s);

void HySocketRead(void *handle);
void HySocketWrite(void *handle);

void HySocketGetInfo(void *handle);

#ifdef __cplusplus
}
#endif

#endif
