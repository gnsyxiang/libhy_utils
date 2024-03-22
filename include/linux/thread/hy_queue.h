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

#include <hy_os_type/hy_type.h>

/**
 * @brief 配置参数
 */
typedef struct {
    hy_u32_t                capacity;               ///< 总容量（程序里会主动对齐到2N次幂）
} HyQueueSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyQueueSaveConfig_s     save_c;                 ///< 配置参数
} HyQueueConfig_s;

typedef struct HyQueue_s HyQueue_s;

/**
 * @brief 创建队列模块
 *
 * @param queue_c 配置参数，详见HyQueueConfig_s
 *
 * @return 成功返回句柄，失败返回NULL
 */
HyQueue_s *HyQueueCreate(HyQueueConfig_s *queue_c);

/**
 * @brief 创建队列模块宏
 *
 * @param _capacity 总容量
 *
 * @return 成功返回句柄，失败返回NULL
 */
#define HyQueueCreate_m(_capacity)                  \
({                                                  \
    HyQueueConfig_s _queue_c;                       \
    HY_MEMSET(&_queue_c, sizeof(_queue_c));         \
    _queue_c.save_c.capacity = _capacity;           \
    HyQueueCreate(&_queue_c);                       \
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
 * @param buf 入队数据
 * @param len 入队长度
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyQueueWrite(HyQueue_s *handle, const void *buf, hy_u32_t len);

/**
 * @brief 从队列中读取数据
 *
 * @param handle 句柄
 * @param buf 出队数据
 * @param len 出队长度
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyQueueRead(HyQueue_s *handle, void *buf, hy_u32_t len);

/**
 * @brief 从队列中读取元素，不删除数据
 *
 * @param handle 句柄
 * @param buf 出队数据
 * @param len 出队长度
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyQueueReadPeek(HyQueue_s *handle, void *buf, hy_u32_t len);

/**
 * @brief 从队列中删除元素
 *
 * @param handle 句柄
 * @param len 删除的个数
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyQueueRemove(HyQueue_s *handle, hy_u32_t len);

/**
 * @brief 获取队列的长度
 *
 * @param handle 句柄
 *
 * @return 返回队列的长度
 */
hy_s32_t HyQueueLenGet(HyQueue_s *handle);

/**
 * @brief 唤醒队列
 *
 * @param handle 句柄
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyQueueWakeup(HyQueue_s *handle);

#ifdef __cplusplus
}
#endif

#endif

