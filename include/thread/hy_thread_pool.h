/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_thread_pool.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    24/04 2023 10:51
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        24/04 2023      create the file
 * 
 *     last modified: 24/04 2023 10:51
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_THREAD_POOL_H_
#define __LIBHY_UTILS_INCLUDE_HY_THREAD_POOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include "hy_type.h"

/**
 * @brief 任务回调函数
 *
 * note:
 * args: 任务的回调参数
 * run_befor_cb_args: 任务调度前初始化返回的参数
 */
typedef void (*HyThreadPoolsTaskCb_t)(void *args, void *run_befor_cb_args);

/**
 * @brief 任务
 */
typedef struct {
    HyThreadPoolsTaskCb_t   task_cb;                        ///< 回调函数
    void                    *args;                          ///< 回调函数的参数
} HyThreadPoolsTask_s;

/**
 * @brief 任务调度前的初始化
 */
typedef void *(*HyThreadPoolRunBeforCb_t)(void *args);

/**
 * @brief 任务调度后的资源释放
 */
typedef void (*HyThreadPoolRunAfterCb_t)(void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    HyThreadPoolRunBeforCb_t    run_befor_cb;               ///< 任务调度前初始化
    HyThreadPoolRunAfterCb_t    run_after_cb;               ///< 任务调度后释放资源
    void                        *run_befor_args;            ///< 任务初始化后参数回调

    hy_s32_t                    thread_cnt_min;             ///< 创建最小线程数
    hy_s32_t                    thread_cnt_max;             ///< 创建最大线程数
} HyThreadPoolSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyThreadPoolSaveConfig_s    save_c;                     ///< 配置参数，详见HyThreadPoolSaveConfig_s

    hy_u32_t                    task_item_cnt;              ///< 最多可容纳的任务数
} HyThreadPoolConfig_s;

typedef struct HyThreadPools_s HyThreadPools_s;

/**
 * @brief 创建线程池模块
 *
 * @param thread_pool_c 配置参数，详见HyThreadPoolConfig_s
 * @return 成功返回句柄，失败返回NULL
 */
HyThreadPools_s *HyThreadPoolCreate(HyThreadPoolConfig_s *thread_pool_c);

/**
 * @brief 销毁线程池模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyThreadPoolDestroy(HyThreadPools_s **handle_pp);

/**
 * @brief 向线程池中加入任务
 *
 * @param handle 句柄
 * @param task 任务结构体的地址，详见HyThreadPoolsTask_s
 */
void HyThreadPoolAddTask(HyThreadPools_s *handle, HyThreadPoolsTask_s *task);

/**
 * @brief 获取工作的线程的个数
 *
 * @param handle 句柄
 * @return 返回工作的线程的个数
 */
hy_s32_t HyThreadPoolGetBusyNum(HyThreadPools_s* handle);

/**
 * @brief 获取存活的线程的个数
 *
 * @param handle 句柄
 * @return 返回活着的线程的个数
 */
hy_s32_t HyThreadPoolGetAliveNum(HyThreadPools_s* handle);

#ifdef __cplusplus
}
#endif

#endif
