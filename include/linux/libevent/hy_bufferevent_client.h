/*
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_bufferevent_client.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    04/05 2023 17:29
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        04/05 2023      create the file
 * 
 *     last modified: 04/05 2023 17:29
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_BUFFEREVENT_CLIENT_H_
#define __LIBHY_UTILS_INCLUDE_HY_BUFFEREVENT_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_type.h"

enum HyBuffereventClientFlag_e {
    HY_BUFFEREVENT_CLIENT_FLAG_DISCONNECTED,
    HY_BUFFEREVENT_CLIENT_FLAG_CONNECTED,
};

/**
 * @brief 接收数据回调
 */
typedef void (*HyBuffereventClientReadCb_t)(void *buf, hy_u32_t len, void *args);

/**
 * @brief 事件回调函数
 */
typedef void (*HyBuffereventClientEventCb_t)(hy_s32_t flag, void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    char                            *ip;        ///< ip地址
    hy_u16_t                        port;       ///< 端口

    HyBuffereventClientEventCb_t    event_cb;   ///< 事件回调函数
    HyBuffereventClientReadCb_t     read_cb;    ///< 读回调函数
    void                            *args;      ///< 参数
} HyBuffereventClientSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyBuffereventClientSaveConfig_s save_c;     ///< 配置参数
} HyBuffereventClientConfig_s;

typedef struct HyBuffereventClient_s HyBuffereventClient_s;

/**
 * @brief 创建buffereventclient模块
 *
 * @param client_c 配置参数
 * @return 成功返回句柄，失败返回NULL
 */
HyBuffereventClient_s *HyBuffereventClientCreate(HyBuffereventClientConfig_s *client_c);

/**
 * @brief 销毁buffereventclient模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyBuffereventClientDestroy(HyBuffereventClient_s **handle_pp);

/**
 * @brief 发送数据
 *
 * @param handle 句柄
 * @param buf 数据地址
 * @param len 数据长度
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyBuffereventClientWrite(HyBuffereventClient_s *handle, void *buf, hy_u32_t len);

#ifdef __cplusplus
}
#endif

#endif

