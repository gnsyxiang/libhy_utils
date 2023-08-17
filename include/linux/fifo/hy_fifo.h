/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_fifo.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 10:48
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 10:48
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_FIFO_H_
#define __LIBHY_UTILS_INCLUDE_HY_FIFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_type.h"

/**
 * @brief 配置参数
 */
typedef struct {
    hy_u32_t            len;            ///< fifo数据空间长度
} HyFifoSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyFifoSaveConfig_s  save_c;         ///< 配置参数
} HyFifoConfig_s;

typedef struct HyFifo_s HyFifo_s;

/**
 * @brief 创建无锁fifo模块
 *
 * @param fifo_c 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 *
 * @note
 * 1，使用该套函数时，需要开辟的空间大一点，否则在写入的时候会出现数据被截断。
 * 2，只适用于单个生产者和单个消费者模型
 */
HyFifo_s *HyFifoCreate(HyFifoConfig_s *fifo_c);

/**
 * @brief 创建无锁fifo模块宏
 *
 * @param _len fifo长度
 *
 * @return 成功返回句柄，失败返回NULL
 */
#define HyFifoCreate_m(_len)                        \
    ({                                              \
        HyFifoConfig_s fifo_c;                      \
        HY_MEMSET(&fifo_c, sizeof(fifo_c));         \
        fifo_c.save_c.len       = _len;             \
        HyFifoCreate(&fifo_c);                      \
     })

/**
 * @brief 销毁无锁fifo模块
 *
 * @param handle_pp fifo句柄的地址（二级指针）
 */
void HyFifoDestroy(HyFifo_s **handle_pp);

/**
 * @brief 向fifo中写入数据
 *
 * @param handle 句柄
 * @param buf 数据
 * @param len 大小
 *
 * @return 成功返回写入的大小
 *
 * @note 如果此时的空间不足以存下len数据，会尽量存下最多，其余的被丢弃
 */
hy_s32_t HyFifoWrite(HyFifo_s *handle, const void *buf, hy_u32_t len);

/**
 * @brief 从fifo中读取数据
 *
 * @param handle 句柄
 * @param buf 数据
 * @param len 大小
 *
 * @return 成功返回读取到的字节数，返回为0表示fifo为空
 */
hy_s32_t HyFifoRead(HyFifo_s *handle, void *buf, hy_u32_t len);

/**
 * @brief 从fifo中读取数据
 *
 * @param handle 句柄
 * @param buf 数据
 * @param len 大小
 *
 * @return 成功返回读取到的字节数，失败返回-1
 *
 * @note
 * 1, 当fifo为空时，直接返回0
 * 2, 该操作不会删除数据
 */
hy_s32_t HyFifoReadPeek(HyFifo_s *handle, void *buf, hy_u32_t len);

/**
 * @brief 从fifo中删除数据
 *
 * @param handle 句柄
 * @param len 长度
 *
 * @return 成功返回删除的字节数，失败返回-1
 */
hy_s32_t HyFifoReadDel(HyFifo_s *handle, hy_u32_t len);

/**
 * @brief 复位fifo
 *
 * @param handle 句柄
 */
void HyFifoReset(HyFifo_s *handle);

/**
 * @brief 打印整个fifo
 *
 * @param handle 句柄
 */
void HyFifoDumpAll(HyFifo_s *handle);

/**
 * @brief 打印fifo中的内容
 *
 * @param handle 句柄
 */
void HyFifoDumpContent(HyFifo_s *handle);

/**
 * @brief 获取fifo中未使用的长度
 *
 * @param handle 句柄
 * @return 成功返回对应值，失败返回-1
 */
hy_s32_t HyFifoGetFreeLen(HyFifo_s *handle);

/**
 * @brief 获取fifo使用的长度
 *
 * @param handle 句柄
 * @return 成功返回对应值，失败返回-1
 */
hy_s32_t HyFifoGetUsedLen(HyFifo_s *handle);

/**
 * @brief 获取fifo使用的长度
 *
 * @param handle 句柄
 * @return 成功返回对应值，失败返回-1
 */
hy_s32_t HyFifoGetTotalLen(HyFifo_s *handle);

/**
 * @brief fifo是否为空
 *
 * @param handle 句柄
 *
 * @return 空返回1，没空返回0，失败返回-1
 */
hy_s32_t HyFifoIsEmpty(HyFifo_s *handle);

/**
 * @brief fifo是否满
 *
 * @param handle 句柄
 *
 * @return 满返回1，没满返回0，失败返回-1
 */
hy_s32_t HyFifoIsFull(HyFifo_s *handle);

#ifdef __cplusplus
}
#endif

#endif

