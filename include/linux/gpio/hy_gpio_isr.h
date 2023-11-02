/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_gpio_isr.h
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
#ifndef __LIBHY_UTILS_INCLUDE_HY_GPIO_ISR_H_
#define __LIBHY_UTILS_INCLUDE_HY_GPIO_ISR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_log/hy_type.h>

/**
 * @brief 中断回调函数
 */
typedef void (*HyGpioIsrCb_t)(hy_s32_t level, void *args);

/**
 * @brief 中断超时回调函数
 */
typedef void (*HyGpioIsrTimeoutCb_t)(void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    hy_s32_t                timeout_ms;             ///< 中断超时时间，-1为阻塞

    HyGpioIsrCb_t           gpio_isr_cb;            ///< 中断回调函数
    HyGpioIsrTimeoutCb_t    gpio_isr_timeout_cb;    ///< 中断超时回调函数
    void                    *args;                  ///< 中断回调函数参数
} HyGpioIsrSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyGpioIsrSaveConfig_s   save_c;                 ///< 配置参数

    hy_s32_t                gpio;                   ///< gpio引脚号
    hy_s32_t                direction;              ///< 输入输出
    hy_s32_t                active_val;             ///< 有效电平
    hy_s32_t                trigger;                ///< 触发方式
} HyGpioIsrConfig_s;

typedef struct HyGpioIsr_s HyGpioIsr_s;

/**
 * @brief 创建周期性获取gpio中断模块
 *
 * @param gpio_isr_c 配置参数，详见HyGpioIsrConfig_s
 *
 * @return 成功返回句柄，失败返回NULL
 */
HyGpioIsr_s *HyGpioIsrCreate(HyGpioIsrConfig_s *gpio_isr_c);

/**
 * @brief 销毁周期性获取gpio中断模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyGpioIsrDestroy(HyGpioIsr_s **handle_pp);

/**
 * @brief 设置超时时间
 *
 * @param handle 句柄
 * @param timeout_ms 超时时间，单位ms
 */
void HyGpioIsrSetTimeout(HyGpioIsr_s *handle, hy_u32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif

