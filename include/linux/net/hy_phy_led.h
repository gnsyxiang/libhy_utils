/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_phy_led.h
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
#ifndef __LIBHY_UTILS_INCLUDE_HY_PHY_LED_H_
#define __LIBHY_UTILS_INCLUDE_HY_PHY_LED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_os_type/hy_type.h>

#define HY_PHY_LED_DEV_NAME_LEN_MAX     (16)
#define HY_PHY_LED_REG_VAL_GROUP        (2)

/**
 * @brief led序号
 */
typedef enum {
    HY_PHY_LED_NUM_0,               ///< led0
    HY_PHY_LED_NUM_1,               ///< led1

    HY_PHY_LED_NUM_MAX,
} HyPHYLedNum_e;

/**
 * @brief led模式
 */
typedef enum {
    HY_PHY_LED_MODE_OFF,            ///< 关闭
    HY_PHY_LED_MODE_ON,             ///< 打开
    HY_PHY_LED_MODE_SLOW_BLINK,     ///< 慢闪
    HY_PHY_LED_MODE_FAST_BLINK,     ///< 快闪

    HY_PHY_LED_MODE_MAX,
} HyPHYLedMode_e;

/**
 * @brief 寄存器结构
 */
typedef struct {
    hy_u32_t                reg;    ///< 寄存器地址
    hy_u32_t                val;    ///< 寄存器值
} HyPHYLedRegVal_s;

/**
 * @brief led模式与寄存器对应
 */
typedef struct {
    hy_s32_t                mode;                               ///< 模式
    HyPHYLedRegVal_s        reg_val[HY_PHY_LED_REG_VAL_GROUP];  ///< 寄存器结构
} HyPHYLedLed_s;

/**
 * @brief 配置参数
 */
typedef struct {
    char                    dev_name[HY_PHY_LED_DEV_NAME_LEN_MAX];          ///< 网卡名称
    HyPHYLedLed_s           led[HY_PHY_LED_NUM_MAX][HY_PHY_LED_MODE_MAX];   ///< led
} HyPHYLedSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyPHYLedSaveConfig_s    save_c; ///< 配置参数
} HyPHYLedConfig_s;

/**
 * @brief 创建有线网尾灯控制模块
 *
 * @param config 配置参数
 *
 * @return 返回模块句柄，失败返回NULL
 */
void *HyPHYLedCreate(HyPHYLedConfig_s *net_led_c);

/**
 * @brief 销毁模块
 *
 * @param handle 模块句柄地址（二级指针）
 */
void HyPHYLedDestroy(void **handle);

/**
 * @brief 设置PHY上的led灯模式
 *
 * @param led led序号
 * @param mode led模式
 */
void HyPHYLedSetLed(HyPHYLedNum_e led, HyPHYLedMode_e mode);

#ifdef __cplusplus
}
#endif

#endif

