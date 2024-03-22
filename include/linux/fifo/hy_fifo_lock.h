/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_fifo_lock.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    15/05 2023 20:11
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        15/05 2023      create the file
 * 
 *     last modified: 15/05 2023 20:11
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_FIFO_LOCK_H_
#define __LIBHY_UTILS_INCLUDE_HY_FIFO_LOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_os_type/hy_type.h>

/**
 * @brief 配置参数
 *
 * capacity: 该参数必须是2^N次幂，否则创建模块时会重新调整该值对齐到2^N次幂
 */
typedef struct {
    hy_u32_t                capacity;                   ///< fifo数据空间长度
} HyFifoLockSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyFifoLockSaveConfig_s  save_c;                     ///< 配置参数
} HyFifoLockConfig_s;

typedef struct HyFifoLock_s HyFifoLock_s;

/**
 * @brief 创建加锁fifo模块
 *
 * @param fifo_lock_c 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
HyFifoLock_s *HyFifoLockCreate(HyFifoLockConfig_s *fifo_lock_c);

/**
 * @brief 创建加锁fifo模块宏
 *
 * @param _capacity fifo长度
 *
 * @return 成功返回句柄，失败返回NULL
 */
#define HyFifoLockCreate_m(_capacity)                   \
({                                                      \
    HyFifoLockConfig_s fifo_lock_c;                     \
    HY_MEMSET(&fifo_lock_c, sizeof(fifo_lock_c));       \
    fifo_lock_c.save_c.capacity = _capacity;            \
    HyFifoLockCreate(&fifo_lock_c);                     \
 })

/**
 * @brief 销毁加锁fifo模块
 *
 * @param handle_pp fifo句柄的地址（二级指针）
 */
void HyFifoLockDestroy(HyFifoLock_s **handle_pp);

/**
 * @brief 向加锁fifo中写入数据
 *
 * @param handle 句柄
 * @param buf 数据
 * @param len 大小
 *
 * @return 
 *    0: 表示已经正在销毁加锁fifo模块;
 *   -1: 表示传入参数为非法值;
 *  > 0: 表示成功写入len字节;
 * 阻塞: 表示空间不足len，等待空间插入，插入成功后返回.
 */
hy_s32_t HyFifoLockWrite(HyFifoLock_s *handle, const void *buf, hy_u32_t len);

/**
 * @brief 从加锁fifo中读取数据
 *
 * @param handle 句柄
 * @param buf 数据
 * @param len 大小
 *
 * @return 
 *    0: 表示已经正在销毁加锁fifo模块;
 *   -1: 表示传入参数为非法值;
 *  > 0: 表示成功读取len字节;
 * 阻塞: 表示数据不足len，等待数据读取，读取成功后返回.
 *
 * @note 该操作不会删除数据
 */
hy_s32_t HyFifoLockReadPeek(HyFifoLock_s *handle, void *buf, hy_u32_t len);

/**
 * @brief 从加锁fifo中读取数据
 *
 * @param handle 句柄
 * @param buf 数据
 * @param len 大小
 *
 * @return 
 *    0: 表示已经正在销毁加锁fifo模块;
 *   -1: 表示传入参数为非法值;
 *  > 0: 表示成功读取len字节;
 * 阻塞: 表示数据不足len，等待数据读取，读取成功后返回.
 */
hy_s32_t HyFifoLockRead(HyFifoLock_s *handle, void *buf, hy_u32_t len);

/**
 * @brief 从加锁fifo中删除数据
 *
 * @param handle 句柄
 * @param len 长度
 *
 * @return 
 *    0: 表示已经正在销毁加锁fifo模块;
 *   -1: 表示传入参数为非法值;
 *  > 0: 表示成功删除len字节;
 * 阻塞: 表示数据不足len，等待数据删除，删除成功后返回.
 */
hy_s32_t HyFifoLockReadDel(HyFifoLock_s *handle, hy_u32_t len);

/**
 * @brief 复位加锁fifo
 *
 * @param handle 句柄
 *
 * @note 获取到锁之后才会复位，否则阻塞等待
 */
void HyFifoLockReset(HyFifoLock_s *handle);

/**
 * @brief 获取加锁fifo使用的长度
 *
 * @param handle 句柄
 * @return 成功返回对应值，失败返回-1
 */
hy_s32_t HyFifoLockGetTotalLen(HyFifoLock_s *handle);

/**
 * @brief 获取fifo使用的长度
 *
 * @param handle 句柄
 * @return 成功返回对应值，失败返回-1
 *
 * @note 没有加锁，获取到的数据在多线程下可能不准
 */
hy_s32_t HyFifoLockGetUsedLen(HyFifoLock_s *handle);

/**
 * @brief 获取fifo中未使用的长度
 *
 * @param handle 句柄
 * @return 成功返回对应值，失败返回-1
 *
 * @note 没有加锁，获取到的数据在多线程下可能不准
 */
hy_s32_t HyFifoLockGetFreeLen(HyFifoLock_s *handle);

/**
 * @brief fifo是否为空
 *
 * @param handle 句柄
 *
 * @return 空返回1，没空返回0，失败返回-1
 *
 * @note 没有加锁，获取到的数据在多线程下可能不准
 */
hy_s32_t HyFifoLockIsEmpty(HyFifoLock_s *handle);

/**
 * @brief fifo是否满
 *
 * @param handle 句柄
 *
 * @return 满返回1，没满返回0，失败返回-1
 *
 * @note 没有加锁，获取到的数据在多线程下可能不准
 */
hy_s32_t HyFifoLockIsFull(HyFifoLock_s *handle);

/**
 * @brief 打印整个fifo
 *
 * @param handle 句柄
 */
void HyFifoLockDumpAll(HyFifoLock_s *handle);

/**
 * @brief 打印fifo中的内容
 *
 * @param handle 句柄
 */
void HyFifoLockDumpContent(HyFifoLock_s *handle);

#ifdef __cplusplus
}
#endif

#endif

