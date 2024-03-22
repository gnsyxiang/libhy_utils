/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_timer.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 13:36
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 13:36
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_TIMER_H_
#define __LIBHY_UTILS_INCLUDE_HY_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_os_type/hy_type.h>

/**
 * @brief 配置参数
 */
typedef struct {
    hy_u32_t                    slot_interval_ms;       ///< 一个格子最小的定时刻度
    hy_u32_t                    slot_num;               ///< 一个周期的总格子数
} HyTimerSaveServerConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyTimerSaveServerConfig_s   save_c;                 ///< 配置参数
} HyTimerServerConfig_s;

typedef struct HyTimerServer_s HyTimerServer_s;

/**
 * @brief 创建定时器服务模块
 *
 * @param timer_server_c 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 *
 * @note 一个系统中只需要创建一个这样的服务，然后增加不同时间的定时器回调即可
 */
HyTimerServer_s *HyTimerServerCreate(HyTimerServerConfig_s *timer_server_c);

//FIXME: 该实现只是实现下这种思路，还有很多的问题：
// 1，效率不行
// 2，定时不准，如定时500ms，实际要到1s才会回调，且每次都有累计误差

/**
 * @brief 销毁定时器服务模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyTimerServerDestroy(HyTimerServer_s **handle_pp);

/**
 * @brief 定时器模式
 */
typedef enum {
    HY_TIMER_MODE_ONCE,                                 ///< 单次
    HY_TIMER_MODE_REPEAT,                               ///< 循环
} HyTimerMode_e;

/**
 * @brief 回调函数
 *
 * @param args 上层传递参数
 */
typedef void (*HyTimerCb_t)(void *args);

/**
 * @brief 创建单个定时器结构体
 */
typedef struct {
    hy_u32_t        expires;                            ///< 超时时间ms
    HyTimerMode_e   mode;                               ///< 是否重复

    HyTimerCb_t     timer_cb;                           ///< 定时器回调函数
    void            *args;                              ///< 上层参数
} HyTimerParam_s;

typedef struct HyTimer_s HyTimer_s;

/**
 * @brief 增加定时器
 *
 * @param handle 定时器服务句柄
 * @param timer_p 定时器配置参数
 *
 * @return 成功返回定时器句柄，失败返回NULL
 */
HyTimer_s *HyTimerAdd(HyTimerServer_s *handle, HyTimerParam_s *timer_p);

/**
 * @brief 增加定时器宏
 *
 * @param _handle 定时器服务句柄
 * @param _expires 超时时间
 * @param _mode 是否重复
 * @param _timer_cb 回调函数
 * @param _args 上层参数
 *
 * @return 返回定时器句柄
 */
#define HyTimerAdd_m(_handle, _expires, _mode, _timer_cb, _args)    \
({                                                                  \
    HyTimerParam_s _timer_p;                                        \
    _timer_p.expires    = _expires;                                 \
    _timer_p.mode       = _mode;                                    \
    _timer_p.timer_cb   = _timer_cb;                                \
    _timer_p.args       = _args;                                    \
    HyTimerAdd(_handle, &_timer_p);                                 \
 })

/**
 * @brief 删除指定定时器
 *
 * @param handle 定时器服务句柄
 * @param timer_pp 定时器巨鼎的地址（二级指针）
 */
void HyTimerDel(HyTimerServer_s *handle, HyTimer_s **timer_pp);

#ifdef __cplusplus
}
#endif

#endif

