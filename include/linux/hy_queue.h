/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_queue.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    24/04 2023 14:29
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        24/04 2023      create the file
 * 
 *     last modified: 24/04 2023 14:29
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_QUEUE_H_
#define __LIBHY_UTILS_INCLUDE_HY_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_type.h"

/**
 * @brief 配置参数
 */
typedef struct {
    hy_u32_t item_len;                  ///< 单个item长度
    hy_u32_t item_cnt;                  ///< item个数（程序里会主动对齐到2N次幂）
} HyQueueSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyQueueSaveConfig_s  save_c;        ///< 配置参数
} HyQueueConfig_s;

typedef struct HyQueue_s HyQueue_s;

/**
 * @brief 创建队列模块
 *
 * @param queue_c 配置参数，详见HyQueueConfig_s
 * @return 成功返回句柄，失败返回NULL
 */
HyQueue_s *HyQueueCreate(HyQueueConfig_s *queue_c);

/**
 * @brief 创建队列模块宏
 *
 * @param _item_cnt item个数
 * @param _item_len 单个item长度
 * @return 成功返回句柄，失败返回NULL
 */
#define HyQueueCreate_m(_item_cnt, _item_len)       \
    ({                                              \
        HyQueueConfig_s queue_c;                    \
        HY_MEMSET(&queue_c, sizeof(queue_c));       \
        queue_c.save_c.item_cnt = _item_cnt;        \
        queue_c.save_c.item_len = _item_len;        \
        HyQueueCreate(&queue_c);                    \
     })

/**
 * @brief 销毁队列模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyQueueDestroy(HyQueue_s **handle_pp);

/**
 * @brief 向队列中写入数据
 *
 * @param handle 句柄
 * @param item 入队数据
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyQueueWrite(HyQueue_s *handle, const void *item);

/**
 * @brief 从队列中读取数据
 *
 * @param handle 句柄
 * @param item item出队数据
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyQueueRead(HyQueue_s *handle, void *item);

/**
 * @brief 从队列中读取元素，不删除数据
 *
 * @param handle 句柄
 * @param item item出队数据
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyQueueReadPeek(HyQueue_s *handle, void *item);

/**
 * @brief 从队列中删除元素
 *
 * @param handle 句柄
 * @param item_cnt 删除的个数
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyQueueReadDel(HyQueue_s *handle, hy_u32_t item_cnt);

/**
 * @brief 获取队列中的个数
 *
 * @param handle 句柄
 * @return 返回队列中的个数
 */
hy_s32_t HyQueueGetItemCount(HyQueue_s *handle);

/**
 * @brief 唤醒队列
 *
 * @param handle 句柄
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyQueueWakeup(HyQueue_s *handle);

#ifdef __cplusplus
}
#endif

#endif

