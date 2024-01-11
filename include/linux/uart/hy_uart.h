/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_uart.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    06/05 2023 15:57
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        06/05 2023      create the file
 * 
 *     last modified: 06/05 2023 15:57
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_UART_H_
#define __LIBHY_UTILS_INCLUDE_HY_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_os_type/hy_os_type.h>

/**
 * @brief 波特率
 */
typedef enum {
    HY_UART_RATE_1200,
    HY_UART_RATE_2400,
    HY_UART_RATE_4800,
    HY_UART_RATE_9600,
    HY_UART_RATE_19200,
    HY_UART_RATE_38400,
    HY_UART_RATE_57600,
    HY_UART_RATE_115200,
    HY_UART_RATE_230400,
    HY_UART_RATE_460800,
    HY_UART_RATE_500000,
    HY_UART_RATE_576000,
    HY_UART_RATE_921600,
    HY_UART_RATE_1000000,
    HY_UART_RATE_1152000,
    HY_UART_RATE_1500000,
    HY_UART_RATE_2000000,
    HY_UART_RATE_2500000,
    HY_UART_RATE_3000000,
    HY_UART_RATE_3500000,
    HY_UART_RATE_4000000,
} HyUartSpeed_e;

/**
 * @brief 流控
 */
typedef enum {
    HY_UART_FLOW_CONTROL_NONE,
    HY_UART_FLOW_CONTROL_SOFT,
    HY_UART_FLOW_CONTROL_HARDWARE,
} HyUartFlowControl_e;

/**
 * @brief 发送数据位数
 */
typedef enum {
    HY_UART_DATA_BIT_5,
    HY_UART_DATA_BIT_6,
    HY_UART_DATA_BIT_7,
    HY_UART_DATA_BIT_8,
} HyUartDataBit_e;

/**
 * @brief 是否校验
 */
typedef enum {
    HY_UART_PARITY_NONE,
    HY_UART_PARITY_ODD,
    HY_UART_PARITY_EVEN,
} HyUartParityType_e;

/**
 * @brief 停止位
 */
typedef enum {
    HY_UART_STOP_BIT_1,
    HY_UART_STOP_BIT_2,
} HyUartStopBit_e;

/**
 * @brief 配置参数
 */
typedef struct {
    const char          *dev_path;      ///< 串口路径

    HyUartSpeed_e       speed;          ///< 波特率
    HyUartFlowControl_e flow_control;   ///< 流控
    HyUartDataBit_e     data_bit;       ///< 发送数据位数
    HyUartParityType_e  parity_type;    ///< 是否校验
    HyUartStopBit_e     stop_bit;       ///< 停止位数
} HyUartSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyUartSaveConfig_s save_c;          ///< 配置参数
} HyUartConfig_s;

typedef struct HyUart_s HyUart_s;

/**
 * @brief 创建串口模块
 *
 * @param uart_c 配置参数，详见HyUartConfig_s
 * @return 成功返回句柄，失败返回NULL
 */
HyUart_s *HyUartCreate(HyUartConfig_s *uart_c);

/**
 * @brief 销毁串口模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyUartDestroy(HyUart_s **handle_pp);

/**
 * @brief 向串口写入数据
 *
 * @param handle 句柄
 * @param buf 数据地址
 * @param len 数据长度
 * @return 成功返回写入的字节数，失败返回-1，写中断返回0
 */
hy_s32_t HyUartWrite(HyUart_s *handle, const void *buf, hy_u32_t len);

/**
 * @brief 向串口写入指定长度的数据
 *
 * @param handle 句柄
 * @param buf 数据地址
 * @param len 数据长度
 * @return 成功返回写入的长度，失败返回-1
 */
hy_s32_t HyUartWriteN(HyUart_s *handle, const void *buf, hy_u32_t len);

/**
 * @brief 从串口读取数据
 *
 * @param handle 句柄
 * @param buf 数据地址
 * @param len 数据长度
 * @return 成功返回读到的字节数，失败返回-1，被中断返回0
 */
hy_s32_t HyUartRead(HyUart_s *handle, void *buf, hy_u32_t len);

/**
 * @brief 从串口中读取指定长度的数据
 *
 * @param handle 句柄
 * @param buf 数据地址
 * @param len 数据长度
 * @return 成功返回指定长度，失败返回-1
 */
hy_s32_t HyUartReadN(HyUart_s *handle, void *buf, hy_u32_t len);

#ifdef __cplusplus
}
#endif

#endif

