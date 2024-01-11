/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_thread_mutex.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 19:40
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 19:40
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_THREAD_MUTEX_H_
#define __LIBHY_UTILS_INCLUDE_HY_THREAD_MUTEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include <hy_os_type/hy_os_type.h>

#include "hy_assert.h"

/**
 * @brief 配置参数
 */
typedef struct {
    hy_s32_t                    reserved;                   ///< 预留
} HyThreadMutexSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyThreadMutexSaveConfig_s   save_c;                     ///< 配置参数
} HyThreadMutexConfig_s;

typedef struct HyThreadMutex_s {
    pthread_mutex_t             mutex;
} HyThreadMutex_s;

/**
 * @brief 创建线程锁模块
 *
 * @param mutex_c 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
HyThreadMutex_s *HyThreadMutexCreate(HyThreadMutexConfig_s *mutex_c);

/**
 * @brief 创建线程锁模块宏
 *
 * @param mutex_c 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
#define HyThreadMutexCreate_m()                             \
({                                                          \
    HyThreadMutexConfig_s _thread_mutex_c;                  \
    HY_MEMSET(&_thread_mutex_c, sizeof(_thread_mutex_c));   \
    HyThreadMutexCreate(&_thread_mutex_c);                  \
 })

/**
 * @brief 销毁线程锁模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyThreadMutexDestroy(HyThreadMutex_s **handle_pp);

/**
 * @brief 加锁
 *
 * @param handle 句柄
 *
 * @return 成功返回0，失败返回其他值
 */
static inline hy_s32_t HyThreadMutexLock(HyThreadMutex_s *handle)
{
    HY_ASSERT(handle);

    return pthread_mutex_lock(&handle->mutex) == 0 ? 0 : -1;
}

#define HyThreadMutexLock_m(_handle)                        \
do {                                                        \
    if (0 != HyThreadMutexLock(_handle)) {                  \
        LOGES("HyThreadMutexLock failed \n");               \
    }                                                       \
} while (0)

/**
 * @brief 开锁
 *
 * @param handle 句柄
 *
 * @return 成功返回0，失败返回其他值
 */
static inline hy_s32_t HyThreadMutexUnLock(HyThreadMutex_s *handle)
{
    HY_ASSERT(handle);

    return pthread_mutex_unlock(&handle->mutex) == 0 ? 0 : -1;
}

#define HyThreadMutexUnLock_m(_handle)                      \
do {                                                        \
    if (0 != HyThreadMutexUnLock(_handle)) {                \
        LOGES("HyThreadMutexUnLock failed \n");             \
    }                                                       \
} while (0)

/**
 * @brief 尝试开锁
 *
 * @param handle 句柄
 *
 * @return 成功返回0，失败返回其他值
 */
static inline hy_s32_t HyThreadMutexTryLock(HyThreadMutex_s *handle)
{
    HY_ASSERT(handle);

    return pthread_mutex_trylock(&handle->mutex) == 0 ? 0 : -1;
}

#define HyThreadMutexTryLock_m(_handle)                     \
do {                                                        \
    if (0 != HyThreadMutexTryLock(_handle)) {               \
        LOGES("HyThreadMutexTryLock failed \n");            \
    }                                                       \
} while (0)

/**
 * @brief 获取锁
 *
 * @param handle 句柄
 *
 * @return 成功返回锁的地址，失败返回NULL
 */
static inline void *HyThreadMutexGetLock(HyThreadMutex_s *handle)
{
    HY_ASSERT(handle);

    return &handle->mutex;
}

#ifdef __cplusplus
}
#endif

#endif

