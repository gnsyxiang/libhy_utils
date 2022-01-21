/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_thread_pool.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    29/12 2021 15:23
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        29/12 2021      create the file
 * 
 *     last modified: 29/12 2021 15:23
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_THREAD_POOL_H_
#define __LIBHY_UTILS_INCLUDE_HY_THREAD_POOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

/**
 * @brief 线程池错误码
 */
typedef enum {
    HY_THREAD_POOL_OK               = 0,
    HY_THREAD_POOL_INVALID          = -1,
    HY_THREAD_POOL_TASK_FULL        = -2,
    HY_THREAD_POOL_SHUT_DOWN        = -3,
    HY_THREAD_POOL_SYSCALL_FAILD    = -4,
} HyThreadPoolError_e;

/**
 * @brief 线程池退出方式
 */
typedef enum {
    HY_THREAD_POOL_DESTROY_RUNNING,                 ///< 运行态

    HY_THREAD_POOL_DESTROY_IMMEDIATE,               ///< 立刻退出
    HY_THREAD_POOL_DESTROY_GRACEFUL,                ///< 等待所有任务执行完后退出
} HyThreadPoolDestroyFlag_e;

/**
 * @brief 任务回调函数
 *
 * @param 上层传递参数
 */
typedef void (*HyThreadPoolTaskCb_t)(void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    hy_u32_t                    task_max_cnt;       ///< 保存的最大任务数
    hy_u32_t                    thread_max_cnt;     ///< 同时运行的最大线程数

    HyThreadPoolDestroyFlag_e   shutdown_flag:2;    ///< 线程池退出方式
    hy_s32_t                    reserved;           ///< 预留
} HyThreadPoolConfig_s;

/**
 * @brief 创建线程池
 *
 * @param config 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
void *HyThreadPoolCreate(HyThreadPoolConfig_s *config);

/**
 * @brief 销毁线程池
 *
 * @param handle 句柄的地址(二级指针)
 */
void HyThreadPoolDestroy(void **handle);

/**
 * @brief 向线程池中添加任务
 *
 * @param handle 句柄
 * @param task_cb 任务回调函数
 * @param args 上层传递参数
 *
 * @return 添加成功返回0，否则返回其他值
 */
hy_s32_t HyThreadPoolAdd(void *handle,
        HyThreadPoolTaskCb_t task_cb, void *args);

/**
 * @brief 创建线程池
 *
 * @param _shutdown_flag 线程池退出方式
 * @param _thread_max_cnt 同时运行的最大线程数
 * @param _task_max_cnt 保存的最大任务数
 *
 * @return 成功返回句柄，失败返回NULL
 */
#define HyThreadPoolCreate_m(_shutdown_flag, _thread_max_cnt, _task_max_cnt)    \
    ({                                                                          \
        HyThreadPoolConfig_s __config;                                          \
        __config.shutdown_flag      = _shutdown_flag;                           \
        __config.thread_max_cnt     = _thread_max_cnt;                          \
        __config.task_max_cnt       = _task_max_cnt;                            \
        HyThreadPoolCreate(&__config);                                          \
     })

#ifdef __cplusplus
}
#endif

#endif
