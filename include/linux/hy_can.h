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

#include <hy_log/hy_type.h>

typedef enum {
    HY_CAN_SPEED_5K,
    HY_CAN_SPEED_10K,
    HY_CAN_SPEED_20K,
    HY_CAN_SPEED_40K,
    HY_CAN_SPEED_50K,
    HY_CAN_SPEED_80K,
    HY_CAN_SPEED_100K,
    HY_CAN_SPEED_125K,
    HY_CAN_SPEED_200K,
    HY_CAN_SPEED_250K,
    HY_CAN_SPEED_400K,
    HY_CAN_SPEED_500K,
    HY_CAN_SPEED_666K,
    HY_CAN_SPEED_800K,
    HY_CAN_SPEED_1000K,
} HyCanSpeed_e;

/**
 * @brief 过滤器设置
 */
typedef enum {
    HY_CAN_FILTER_TYPE_PASS,
    HY_CAN_FILTER_TYPE_REJECT,
} HyCanFilterType_e;

/**
 * @brief 配置参数
 */
typedef struct {
    hy_s32_t            can_id;                 ///< 自身的can id
    const char          *name;                  ///< can接口
} HyCanSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyCanSaveConfig_s   save_c;                 ///< 配置参数

    hy_u32_t            *filter_id;             ///< 需要过滤的can_id
    hy_u32_t            filter_id_cnt;          ///< can_id个数
    HyCanFilterType_e   filter;                 ///< 过滤或取反

    hy_s32_t            file_block;             ///< 0为阻塞状态，1为非阻塞状态
    HyCanSpeed_e        speed;                  ///< 速度
} HyCanConfig_s;

typedef struct HyCan_s HyCan_s;

/**
 * @brief 创建can模块
 *
 * @param can_c 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
HyCan_s *HyCanCreate(HyCanConfig_s *can_c);

/**
 * @brief 销毁can模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyCanDestroy(HyCan_s **handle_pp);

/**
 * @brief 发送数据
 *
 * @param handle 句柄
 * @param buf 数据的地址
 * @param len 数据的长度
 *
 * @return 成功返回写入的数据，失败返回-1 
 */
hy_s32_t HyCanWrite(HyCan_s *handle, char *buf, hy_u32_t len);

/**
 * @brief 读取数据
 *
 * @param handle 句柄
 * @param buf 数据地址
 * @param len 需要读取的数据长度
 * @return 成功返回读到的字节数，失败返回-1
 */
hy_s32_t HyCanRead(HyCan_s *handle, void *buf, hy_u32_t len);

/**
 * @brief 读取数据，超时等待
 *
 * @param handle 句柄
 * @param buf 数据地址
 * @param len 需要读取的数据长度
 * @param ms 超时时间，单位: 毫秒
 * @return 
 *   1，成功返回读到的字节数
 *   2，失败返回-1
 *   3，超时返回0
 */
hy_s32_t HyCanReadTimeout(HyCan_s *handle, void *buf, hy_u32_t len, hy_u32_t ms);

/**
 * @brief 获取can的fd句柄
 *
 * @param handle 句柄
 * @return 成功返回句柄，失败返回-1
 */
hy_s32_t HyCanGetFd(HyCan_s *handle);

#ifdef __cplusplus
}
#endif

#endif
