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

#include <stdio.h>
#include <stdint.h>

#include "hy_hal/hy_type.h"

/**
 * @brief 获取FIFO相关信息
 */
typedef enum {
    HY_FIFO_INFO_TOTAL_LEN,             ///< 获取FIFO总长度
    HY_FIFO_INFO_USED_LEN,              ///< 获取FIFO使用长度
    HY_FIFO_INFO_FREE_LEN,              ///< 获取FIFO空闲长度

    HY_FIFO_INFO_MAX,
} HyFifoInfoType_t;

/**
 * @brief 打印fifo相关信息
 */
typedef enum {
    HY_FIFO_DUMP_ALL,                   ///< 打印fifo全部信息
    HY_FIFO_DUMP_CONTENT,               ///< 打印fifo里面的内容

    HY_FIFO_DUMP_MAX,
} HyFifoDumpType_t;

/**
 * @brief 模块配置参数
 */
typedef struct {
    hy_u32_t len;                       ///< fifo数据空间长度
} HyFifoSaveConfig_t;

/**
 * @brief 模块配置参数
 */
typedef struct {
    HyFifoSaveConfig_t save_config;     ///< 参数，详见HyFifoSaveConfig_t
} HyFifoConfig_t;

/**
 * @brief 创建fifo模块
 *
 * @param config 配置参数，详见HyFifoConfig_t
 *
 * @return 成功返回fifo句柄，失败返回NULL
 *
 * @note 
 * 1, 该fifo里面没有使用锁，只能使用在一读和一写线程当中，多线程需要外部加锁
 * 2, 如果读取速度很慢且空间开辟的很小，则会丢弃数据
 */
void *HyFifoCreate(HyFifoConfig_t *config);

/**
 * @brief 销毁fifo模块
 *
 * @param handle fifo句柄的地址
 */
void HyFifoDestroy(void **handle);

/**
 * @brief 清除fifo中的数据
 *
 * @param handle 操作fifo句柄
 */
void HyFifoClean(void *handle);

/**
 * @brief 从fifo中删除数据
 *
 * @param handle 操作fifo句柄
 * @param len 删除数据的长度
 *
 * @return 返回删除的字节数
 */
hy_u32_t HyFifoUpdateOut(void *handle, hy_u32_t len);

/**
 * @brief 向fifo中插入数据
 *
 * @param handle 操作fifo句柄
 * @param buf 插入的数据
 * @param len 插入数据的大小
 *
 * @return 返回成功插入的字节数
 */
hy_u32_t HyFifoWrite(void *handle, void *buf, hy_u32_t len);

/**
 * @brief 从fifo中获取数据
 *
 * @param handle 操作fifo句柄
 * @param buf 被取出的数据
 * @param len 被取出数据的大小
 *
 * @return 返回成功取出的数据
 */
hy_u32_t HyFifoRead(void *handle, void *buf, hy_u32_t len);

/**
 * @brief 从fifo中获取数据
 *
 * @param handle 操作fifo句柄
 * @param buf 被取出的数据
 * @param len 被取出数据的大小
 *
 * @return 返回成功取出的数据
 *
 * note 该操作不会删除数据
 */
hy_u32_t HyFifoReadPeek(void *handle, void *buf, hy_u32_t len);

/**
 * @brief 打印fifo
 *
 * @param handle 操作fifo句柄
 */
void HyFifoDump(void *handle, HyFifoDumpType_t type);

/**
 * @brief 获取FIFO相关信息
 *
 * @param handle 操作fifo句柄
 * @param type 操作类型，详见HyFifoInfoType_t
 *
 * @return 成功返回对应的值，失败有失败打印，且值为0
 */
hy_u32_t HyFifoGetInfo(void *handle, HyFifoInfoType_t type);

/**
 * @brief fifo是否满
 *
 * @param handle 操作fifo句柄
 *
 * @return 满返回1，否则返回0
 */
int32_t HyFifoIsFull(void *handle);

/**
 * @brief fifo是否为空
 *
 * @param handle 操作fifo句柄
 *
 * @return 空返回1，否则返回0
 */
int32_t HyFifoIsEmpty(void *handle);

#ifdef __cplusplus
}
#endif

#endif
