/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_timer_multi_wheel.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    11/04 2022 10:54
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        11/04 2022      create the file
 * 
 *     last modified: 11/04 2022 10:54
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_TIMER_MULTI_WHEEL_H_
#define __LIBHY_UTILS_INCLUDE_HY_TIMER_MULTI_WHEEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_type.h"

/**
 * @brief 
 */
typedef enum {
    HY_TIMER_MULTI_WHEEL_MODE_ONCE,   ///< 
    HY_TIMER_MULTI_WHEEL_MODE_REPEAT,   ///< 

    HY_TIMER_MULTI_WHEEL_MODE_MAX,   ///< 
} HyTimerMultiWheelMode_e;

/**
 * @brief 
 *
 * @param args 
 *
 * @return 
 */
typedef void (*HyTimerMultiWheelCb_t)(void *args);

/**
 * @brief 创建当个定时器结构体
 */
typedef struct {
    hy_u32_t                    expires;   ///< 
    HyTimerMultiWheelMode_e     mode; 

    HyTimerMultiWheelCb_t       timer_cb;
    void                        *args;
} HyTimerMultiWheelConfig_s;

void HyTimerMultiWheelCreate(hy_u32_t tick_ms);
void HyTimerMultiWheelDestroy(void);

void *HyTimerMultiWheelAdd(HyTimerMultiWheelConfig_s *timer_c);

hy_s32_t HyTimerMultiWheelDel(void *handle);

#ifdef __cplusplus
}
#endif

#endif

