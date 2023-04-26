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

#include "hy_type.h"

/**
 * @brief 定时器模式
 */
typedef enum {
    HY_TIMER_MODE_ONCE,             ///< 单次
    HY_TIMER_MODE_REPEAT,           ///< 循环

    HY_TIMER_MODE_MAX,
} HyTimerMode_e;

/**
 * @brief 回调函数
 *
 * @param args 上层传递参数
 *
 * @return 无
 */
typedef void (*HyTimerCb_t)(void *args);

/**
 * @brief 创建单个定时器结构体
 */
typedef struct {
    hy_u32_t        expires;        ///< 超时时间ms
    HyTimerMode_e   mode;           ///< 是否重复

    HyTimerCb_t     timer_cb;       ///< 定时器回调函数
    void            *args;          ///< 上层参数
} HyTimerConfig_s;

/**
 * @brief 创建定时器服务模块
 *
 * @param slot_interval_ms 一个格子最小的定时刻度
 * @param slot_num 一个周期的总格子数
 *
 * @note 只需要调用一次
 */
void HyTimerCreate(hy_u32_t slot_interval_ms, hy_u32_t slot_num);

/**
 * @brief 销毁定时器服务模块
 */
void HyTimerDestroy(void);

/**
 * @brief 增加定时器
 *
 * @param timer_c 定时器配置参数
 *
 * @return 成功返回定时器句柄，失败返回NULL
 */
void *HyTimerAdd(HyTimerConfig_s *timer_c);

/**
 * @brief 增加定时器宏
 *
 * @param _expires 超时时间
 * @param _mode 是否重复
 * @param _timer_cb 回调函数
 * @param _args 上层参数
 *
 * @return 返回定时器句柄
 */
#define HyTimerAdd_m(_expires, _mode, _timer_cb, _args)     \
    ({                                                      \
        HyTimerConfig_s timer_c;                            \
        timer_c.expires     = _expires;                     \
        timer_c.mode        = _mode;                        \
        timer_c.timer_cb    = _timer_cb;                    \
        timer_c.args        = _args;                        \
        HyTimerAdd(&timer_c);                               \
     })

/**
 * @brief 删除指定定时器
 *
 * @param handle 定时器句柄的地址(二级指针)
 */
void HyTimerDel(void **handle);


#ifdef __cplusplus
}
#endif

#endif

