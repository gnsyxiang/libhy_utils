/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_can.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/03 2023 15:04
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/03 2023      create the file
 * 
 *     last modified: 18/03 2023 15:04
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_CAN_H_
#define __LIBHY_UTILS_INCLUDE_HY_CAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/can.h>
#include <linux/can/raw.h>

#include "hy_type.h"

/**
 * @brief can id 传入的最大数组长度
 */
#define HY_CAN_ID_MAX (64)

/**
 * @brief 过滤器设置
 */
typedef enum {
    HY_CAN_FILTER_PASS,
    HY_CAN_FILTER_REJECT,
} HyCanFilter_e;

/**
 * @brief 配置参数
 */
typedef struct {
    hy_u32_t            can_id[HY_CAN_ID_MAX];  ///< 需要过滤的can_id
    hy_u32_t            can_id_cnt;             ///< can_id个数
    HyCanFilter_e       filter;                 ///< 过滤或取反
} HyCanSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyCanSaveConfig_s   save_c;                 ///< 配置参数

    const char          *name;                  ///< can接口
    hy_u32_t            speed;                  ///< 速度
} HyCanConfig_s;

/**
 * @brief 创建can模块
 *
 * @param can_c 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
void *HyCanCreate(HyCanConfig_s *can_c);

/**
 * @brief 销毁can模块
 *
 * @param handle 句柄的地址
 */
void HyCanDestroy(void **handle);

/**
 * @brief 发送数据
 *
 * @param handle 句柄
 * @param tx_frame 数据
 *
 * @return 成功返回写入的数据，失败返回-1 
 */
hy_s32_t HyCanWrite(void *handle, struct can_frame *tx_frame);

/**
 * @brief 发送数据
 *
 * @param handle 句柄
 * @param can_id can id
 * @param buf 数组
 * @param len 数组长度
 *
 * @return 成功返回写入的数据，失败返回-1 
 */
hy_s32_t HyCanWriteBuf(void *handle, hy_u32_t can_id, char *buf, hy_u32_t len);

/**
 * @brief 读取数据
 *
 * @param handle 句柄
 * @param rx_frame 数据
 *
 * @return  成功返回读取到的数据，失败返回-1
 */
hy_s32_t HyCanRead(void *handle, struct can_frame *rx_frame);

/**
 * @brief 获取fd
 *
 * @param handle 句柄
 *
 * @return 返回fd
 */
hy_s32_t HyCanGetFd(void *handle);

#ifdef __cplusplus
}
#endif

#endif
