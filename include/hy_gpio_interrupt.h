/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_gpio_interrupt.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    26/04 2023 19:26
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        26/04 2023      create the file
 * 
 *     last modified: 26/04 2023 19:26
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_GPIO_INTERRUPT_H_
#define __LIBHY_UTILS_INCLUDE_HY_GPIO_INTERRUPT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_type.h"

/**
 * @brief 中断回调函数
 */
typedef void (*HyGpioInterruptCb_t)(hy_s32_t level, void *args);

/**
 * @brief 中断超时回调函数
 */
typedef void (*HyGpioInterruptTimeoutCb_t)(void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    HyGpioInterruptCb_t         gpio_interrupt_cb;          ///< 中断回调函数
    void                        *args;                      ///< 中断回调函数参数
    HyGpioInterruptTimeoutCb_t  gpio_interrupt_timeout_cb;  ///< 中断超时回调函数
    void                        *timeout_args;              ///< 中断超时回调函数参数
    hy_s32_t                    timeout_ms;                 ///< 中断超时时间
} HyGpioInterruptSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyGpioInterruptSaveConfig_s save_c;                     ///< 配置参数

    hy_s32_t gpio;                                          ///< gpio引脚号
    hy_s32_t direction;                                     ///< 输入输出
    hy_s32_t active_val;                                    ///< 有效电平
    hy_s32_t trigger;                                       ///< 触发方式
} HyGpioInterruptConfig_s;

typedef struct HyGpioInterrupt_s HyGpioInterrupt_s;

/**
 * @brief 创建周期性获取gpio中断模块
 *
 * @param config 配置参数，详见HyGpioInterruptConfig_s
 * @return 成功返回句柄，失败返回NULL
 */
HyGpioInterrupt_s *HyGpioInterruptCreate(HyGpioInterruptConfig_s *gpio_interrupt_c);

/**
 * @brief 销毁周期性获取gpio中断模块
 *
 * @param handle 句柄的地址（二级指针）
 */
void HyGpioInterruptDestroy(HyGpioInterrupt_s **handle);

#ifdef __cplusplus
}
#endif

#endif
