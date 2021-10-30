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

#include <stdio.h>
#include <stdint.h>

/**
 * @brief 定时器模式
 */
typedef enum {
    HY_TIMER_MODE_ONCE,                         ///< 单次
    HY_TIMER_MODE_REPEAT,                       ///< 循环

    HY_TIMER_MODE_MAX,
} HY_TIMER_MODE_t;

/**
 * @brief 创建单个定时器结构体
 */
typedef struct {
    size_t  expires;                            ///< 超时时间ms = expires * slot_interval_ms
    int32_t repeat_flag;                        ///< 是否重复，详见HY_TIMER_MODE_t

    void    (*timer_cb)(void *args);            ///< 定时器回调函数
    void    *args;                              ///< 定时器回调函数参数
} HyTimerConfig_t;

/**
 * @brief 配置结构体
 */
typedef struct {
    uint32_t slot_num;                          ///< 一格周期的总格子数
    uint32_t slot_interval_ms;                  ///< 一格子定时的最小刻度
} HyTimerServiceSaveConfig_t;

/**
 * @brief 配置结构体
 */
typedef struct {
    HyTimerServiceSaveConfig_t save_config;     ///< 配置结构体，详见HyTimerServiceSaveConfig_t
} HyTimerServiceConfig_t;

/**
 * @brief 创建定时器服务模块
 *
 * @param config 配置参数，详见HyTimerServiceConfig_t
 *
 * @return 返回定时器服务操作句柄
 */
void *HyTimerCreate(HyTimerServiceConfig_t *config);

/**
 * @brief 销毁定时器服务模块
 *
 * @param handle 定时器服务句柄的地址
 */
void HyTimerDestroy(void **handle);

/**
 * @brief 增加一个定时器
 *
 * @param timer_config 定时器配置参数，详见HyTimerConfig_t
 *
 * @return 返回定时器句柄
 */
void *HyTimerAdd(HyTimerConfig_t *timer_config);

/**
 * @brief 删除指定定时器
 *
 * @param timer_handle 定时器句柄的地址
 */
void HyTimerDel(void **timer_handle);

#ifdef __cplusplus
}
#endif

#endif

