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
 * @brief 获取fifo相关信息
 */
typedef enum {
    HY_FIFO_INFO_TOTAL_LEN,             ///< 获取fifo总长度
    HY_FIFO_INFO_USED_LEN,              ///< 获取fifo使用长度
    HY_FIFO_INFO_FREE_LEN,              ///< 获取fifo空闲长度

    HY_FIFO_INFO_MAX = 0xffffffff,
} HyFifoInfo_e;

/**
 * @brief 打印fifo相关信息
 */
typedef enum {
    HY_FIFO_DUMP_ALL,                   ///< 打印fifo全部信息，按照开辟的空间打印
    HY_FIFO_DUMP_CONTENT,               ///< 打印fifo里面的有效内容

    HY_FIFO_DUMP_MAX = 0xffffffff,
} HyFifoDump_e;

/**
 * @brief 锁状态
 */
typedef enum {
    HY_FIFO_MUTEX_UNLOCK,               ///< 不加锁
    HY_FIFO_MUTEX_LOCK,                 ///< 加锁
} HyFifoMutex_e;

/**
 * @brief 配置参数
 */
typedef struct {
    hy_u32_t            len;            ///< fifo数据空间长度
    HyFifoMutex_e       is_lock;        ///< 是否加锁，多线程需要加锁
} HyFifoSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyFifoSaveConfig_s  save_c;         ///< 配置参数
} HyFifoConfig_s;

/**
 * @brief 创建fifo模块
 *
 * @param config 配置参数
 *
 * @return 成功返回fifo句柄，失败返回NULL
 */
void *HyFifoCreate(HyFifoConfig_s *fifo_c);

/**
 * @brief 创建fifo模块宏
 *
 * @param _len fifo长度
 * @param _is_lock fifo是否加锁
 *
 * @return 成功返回句柄，失败返回NULL
 */
#define HyFifoCreate_m(_len, _is_lock)              \
    ({                                              \
        HyFifoConfig_s fifo_c;                      \
        HY_MEMSET(&fifo_c, sizeof(fifo_c));         \
        fifo_c.save_c.len       = _len;             \
        fifo_c.save_c.is_lock   = _is_lock;         \
        HyFifoCreate(&fifo_c);                      \
     })

/**
 * @brief 销毁fifo模块
 *
 * @param handle fifo句柄的地址（二级指针）
 */
void HyFifoDestroy(void **handle);

/**
 * @brief 向fifo中写入数据
 *
 * @param handle 句柄
 * @param buf 数据
 * @param len 大小
 *
 * @return 成功返回写入的大小，失败返回-1
 *
 * @note 没有足够空间写入时，一个字节也不会写入，直接返回-1
 */
hy_s32_t HyFifoWrite(void *handle, const void *buf, hy_u32_t len);

/**
 * @brief 从fifo中读取数据
 *
 * @param handle 句柄
 * @param buf 数据
 * @param len 大小
 *
 * @return 成功返回读取到的字节数，失败返回-1
 *
 * @note 当fifo为空时，直接返回0
 */
hy_s32_t HyFifoRead(void *handle, void *buf, hy_u32_t len);

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
hy_s32_t HyFifoReadPeek(void *handle, void *buf, hy_u32_t len);

/**
 * @brief 从fifo中删除数据
 *
 * @param handle 句柄
 * @param len 长度
 *
 * @return 成功返回删除的字节数，失败返回-1
 */
hy_s32_t HyFifoReadDel(void *handle, hy_u32_t len);

/**
 * @brief 复位fifo
 *
 * @param handle 句柄
 */
void HyFifoReset(void *handle);

/**
 * @brief 打印fifo
 *
 * @param handle 句柄
 */
void HyFifoDump(void *handle, HyFifoDump_e type);

/**
 * @brief 获取FIFO相关信息
 *
 * @param handle 句柄
 * @param type 操作类型
 *
 * @return 成功返回对应的值，失败返回-1
 */
hy_s32_t HyFifoGetInfo(void *handle, HyFifoInfo_e type);

/**
 * @brief fifo是否满
 *
 * @param handle 句柄
 *
 * @return 满返回1，否则返回0
 */
hy_s32_t HyFifoIsFull(void *handle);

/**
 * @brief fifo是否为空
 *
 * @param handle 句柄
 *
 * @return 空返回1，否则返回0
 */
hy_s32_t HyFifoIsEmpty(void *handle);

#ifdef __cplusplus
}
#endif

#endif

