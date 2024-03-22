/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_epoll.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/03 2023 11:19
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/03 2023      create the file
 * 
 *     last modified: 18/03 2023 11:19
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_EPOLL_H_
#define __LIBHY_UTILS_INCLUDE_HY_EPOLL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/epoll.h>

#include <hy_os_type/hy_type.h>

/**
 * @brief 监控节点的类型
 */
typedef enum {
    HY_EPOLL_FD_TYPE_FILE,          ///< 文件
    HY_EPOLL_FD_TYPE_CAN,           ///< 
    HY_EPOLL_FD_TYPE_CLIENT,        ///< 
    HY_EPOLL_FD_TYPE_SERVER,        ///< 
} HyEpollFdType_e;

/**
 * @brief 回调函数参数
 */
typedef struct {
    hy_s32_t    fd;                 ///< 
    hy_s32_t    fd_type;            ///< 详见HyEpollFdType_e
    char        ip_addr[16];        ///<
    void        *args;              ///< 
} HyEpollEventCbParam_s;

/**
 * @brief 回调函数
 *
 * @param event_cb_param回调函数参数，详见HyEpollEventCbParam_s
 */
typedef void (*HyEpollEventCb_t)(HyEpollEventCbParam_s *event_cb_param);

/**
 * @brief  配置参数
 */
typedef struct {
    hy_u32_t            max_event;          ///< 内核监控的最大数
    HyEpollEventCb_t    event_epoll_cb;     ///< 事件回调函数
} HyEpollSaveConfig_s;

/**
 * @brief  配置参数
 */
typedef struct {
    HyEpollSaveConfig_s save_c;             ///< 配置参数
} HyEpollConfig_s;

/**
 * @brief 创建epoll模块
 *
 * @param config 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
void *HyEpollCreate(HyEpollConfig_s *config);

/**
 * @brief 创建epoll模块宏
 *
 * @param _max_event 内核监控的最大数
 * @param _event_epoll_cb 事件回调函数
 *
 * @return 成功返回句柄，失败返回NULL
 */
#define HyEpollCreate_m(_max_event, _event_epoll_cb)    \
({                                                      \
    HyEpollConfig_s epoll_c;                            \
    HY_MEMSET(&epoll_c, sizeof(epoll_c));               \
    epoll_c.save_c.max_event = _max_event;              \
    epoll_c.save_c.event_epoll_cb = _event_epoll_cb;    \
 })

/**
 * @brief 销毁epoll模块
 *
 * @param handle_pp 句柄的地址
 */
void HyEpollDestroy(void **handle_pp);

/**
 * @brief 向epoll中添加事件
 *
 * @param handle 句柄
 * @param event 事件
 * @param event_cb_param 事件对应回调函数参数
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyEpollAdd(void *handle, hy_s32_t event, hy_s32_t fd,
                    HyEpollEventCbParam_s *event_cb_param);

/**
 * @brief 从epoll中删除事件
 *
 * @param handle 句柄
 * @param fd fd
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyEpollDel(void *handle, hy_s32_t fd);

#ifdef __cplusplus
}
#endif

#endif

