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

#include "hy_hal/hy_type.h"

/**
 * @brief 定时器模式
 */
typedef enum {
    HY_TIMER_MODE_ONCE,                         ///< 单次
    HY_TIMER_MODE_REPEAT,                       ///< 循环

    HY_TIMER_MODE_MAX,
} HyTimerMode_e;

/**
 * @brief 创建单个定时器结构体
 */
typedef struct {
    hy_u32_t        expires;                    ///< 超时时间ms
    HyTimerMode_e   repeat_flag;                ///< 是否重复

    void            (*timer_cb)(void *args);    ///< 定时器回调函数
    void            *args;                      ///< 上层参数
} HyTimerConfig_s;

/**
 * @brief 配置结构体
 */
typedef struct {
    hy_u32_t    slot_num;                       ///< 一个周期的总格子数
    hy_u32_t    slot_interval_ms;               ///< 一个格子定时的最小刻度
} HyTimerServiceSaveConfig_s;

/**
 * @brief 配置结构体
 */
typedef struct {
    HyTimerServiceSaveConfig_s save_config;     ///< 配置结构体
} HyTimerServiceConfig_s;

/**
 * @brief 创建定时器模块
 *
 * @param config 配置参数
 *
 * @return 成功返回定时器模块句柄，失败返回NULL
 */
void *HyTimerCreate(HyTimerServiceConfig_s *config);

/**
 * @brief 销毁定时器模块
 *
 * @param handle 定时器模块句柄的地址(二级指针)
 */
void HyTimerDestroy(void **handle);

/**
 * @brief 增加定时器
 *
 * @param timer_config 定时器配置参数
 *
 * @return 返回定时器句柄
 */
void *HyTimerAdd(HyTimerConfig_s *timer_config);

/**
 * @brief 删除指定定时器
 *
 * @param timer_handle 定时器句柄的地址(二级指针)
 */
void HyTimerDel(void **timer_handle);

/**
 * @brief 增加定时器宏
 *
 * @param _expires 超时时间
 * @param _repeat_flag 是否重复
 * @param _timer_cb 回调函数
 * @param _args 上层参数
 *
 * @return 返回定时器句柄
 */
#define HyTimerAdd_m(_expires, _repeat_flag, _timer_cb, _args)          \
    ({                                                                  \
        HyTimerConfig_s __config;                                       \
        __config.expires        = _expires;                             \
        __config.repeat_flag    = _repeat_flag;                         \
        __config.timer_cb       = _timer_cb;                            \
        __config.args           = _args;                                \
        HyTimerAdd(&__config);                                          \
     })

/**
 * @brief 创建定时器模块宏
 *
 * @param _slot_interval_ms 一个格子定时的最小刻度
 * @param _slot_num 一个周期的总格子数
 *
 * @return 
 */
#define HyTimerCreate_m(_slot_interval_ms, _slot_num)                   \
    ({                                                                  \
        HyTimerServiceConfig_s __config;                                \
        __config.save___config.slot_interval_ms = _slot_interval_ms;    \
        __config.save___config.slot_num         = _slot_num;            \
        HyTimerCreate(&__config);                                       \
     })

#ifdef __cplusplus
}
#endif

#endif
