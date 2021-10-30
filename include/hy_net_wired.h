/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_net_wired.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 11:17
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 11:17
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_NET_WIRED_H_
#define __LIBHY_UTILS_INCLUDE_HY_NET_WIRED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define HY_NET_WIRED_DEV_NAME_LEN_MAX   (16)
#define HY_NET_WIRED_REG_VAL_GROUP      (2)

/**
 * @brief led
 *
 * @note 上层设置led跟具体灯的对应
 */
typedef enum {
    HY_NET_WIRED_LED_0,                     ///< led0
    HY_NET_WIRED_LED_1,                     ///< led1

    HY_NET_WIRED_LED_MAX,
} HY_NET_WIRED_LED_t;

/**
 * @brief led模式
 */
typedef enum {
    HY_NET_WIRED_LED_MODE_OFF,              ///< 关闭
    HY_NET_WIRED_LED_MODE_ON,               ///< 打开
    HY_NET_WIRED_LED_MODE_SLOW_BLINK,       ///< 慢闪
    HY_NET_WIRED_LED_MODE_FAST_BLINK,       ///< 快闪

    HY_NET_WIRED_LED_MODE_MAX,
} HY_NET_WIRED_LED_MODE_t;

/**
 * @brief 寄存器结构
 */
typedef struct {
    uint32_t reg;                           ///< 寄存器地址
    uint32_t val;                           ///< 寄存器值
} HyNetWiredRegVal_t;

/**
 * @brief led模式与寄存器对应
 */
typedef struct {
    int32_t mode;                                               ///< 模式，详见HY_NET_WIRED_LED_MODE_t
    HyNetWiredRegVal_t reg_val[HY_NET_WIRED_REG_VAL_GROUP];     ///< 寄存器，详见HyNetWiredRegVal_t
} HyNetWiredLed_t;

/**
 * @brief 模块配置参数
 */
typedef struct {
    char dev_name[HY_NET_WIRED_DEV_NAME_LEN_MAX];                           ///< 网卡名称
    HyNetWiredLed_t led[HY_NET_WIRED_LED_MAX][HY_NET_WIRED_LED_MODE_MAX];   ///< led，详见HyNetWiredLed_t
} HyNetWiredSaveConfig_t;

/**
 * @brief 模块配置参数
 */
typedef struct {
    HyNetWiredSaveConfig_t save_config;     ///< 模块配置参数，详见HyNetWiredSaveConfig_t
} HyNetWiredConfig_t;

/**
 * @brief 创建有线网络模块
 *
 * @param config 模块配置参数，详见HyNetWiredConfig_t
 *
 * @return 返回模块句柄，失败返回NULL
 */
void *HyNetWiredCreate(HyNetWiredConfig_t *config);

/**
 * @brief 销毁有线网络模块
 *
 * @param handle 模块句柄地址
 */
void HyNetWiredDestroy(void **handle);

/**
 * @brief 设置PHY上的led灯模式
 *
 * @param led led，详见HY_NET_WIRED_LED_t
 * @param mode led模式，详见HY_NET_WIRED_LED_MODE_t
 */
void HyNetWiredSetLed(int32_t led, int32_t mode);

#ifdef __cplusplus
}
#endif

#endif

