/**
 *
 * Copyright (C), 2010-2011,江西科萌科技控股有限公司
 * 
 * @file    hy_thread_pool_dynamic.h
 * @brief   
 * @author  zhenquan.qiu
 * @date    14/08 2023 15:00
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        14/08 2023      create the file
 * 
 *     last modified: 14/08 2023 15:00
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_THREAD_POOL_DYNAMIC_H_
#define __LIBHY_UTILS_INCLUDE_HY_THREAD_POOL_DYNAMIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_type.h"

/**
 * @brief 任务回调函数
 *
 * note:
 * args: 任务的回调参数
 * run_befor_cb_args: 任务调度前初始化返回的参数
 */
typedef void (*HyThreadPoolDynamicTaskCb_t)(void *args, void *run_befor_cb_args);

/**
 * @brief 任务
 */
typedef struct {
    HyThreadPoolDynamicTaskCb_t         task_cb;                ///< 回调函数
    void                                *args;                  ///< 回调函数的参数
} HyThreadPoolTask_s;

/**
 * @brief 任务调度前的初始化
 */
typedef void *(*HyThreadPoolDynamicRunBeforCb_t)(void *args);

/**
 * @brief 任务调度后的资源释放
 */
typedef void (*HyThreadPoolDynamicRunAfterCb_t)(void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    HyThreadPoolDynamicRunBeforCb_t     run_befor_cb;           ///< 任务调度前初始化
    HyThreadPoolDynamicRunAfterCb_t     run_after_cb;           ///< 任务调度后释放资源
    void                                *run_befor_args;        ///< 任务初始化后参数回调

    hy_s32_t                            thread_cnt_min;         ///< 创建最小线程数
    hy_s32_t                            thread_cnt_max;         ///< 创建最大线程数
} HyThreadPoolDynamicSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyThreadPoolDynamicSaveConfig_s     save_c;                 ///< 配置参数，详见HyThreadPoolSaveConfig_s

    hy_u32_t                            task_item_cnt;          ///< 最多可容纳的任务数
} HyThreadPoolDynamicConfig_s;

typedef struct HyThreadPoolDynamic_s HyThreadPoolDynamic_s;

/**
 * @brief 创建线程池模块
 *
 * @param thread_pool_c 配置参数，详见HyThreadPoolConfig_s
 * @return 成功返回句柄，失败返回NULL
 */
HyThreadPoolDynamic_s *HyThreadPoolDynamicCreate(HyThreadPoolDynamicConfig_s *thread_pool_dynamic_c);

/**
 * @brief 销毁线程池模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyThreadPoolDynamicDestroy(HyThreadPoolDynamic_s **handle_pp);

/**
 * @brief 向线程池中加入任务
 *
 * @param handle 句柄
 * @param task 任务结构体的地址，详见HyThreadPoolsTask_s
 */
void HyThreadPoolDynamicAddTask(HyThreadPoolDynamic_s *handle, HyThreadPoolTask_s *task);

/**
 * @brief 获取工作的线程的个数
 *
 * @param handle 句柄
 * @return 返回工作的线程的个数
 */
hy_s32_t HyThreadPoolDynamicGetBusyNum(HyThreadPoolDynamic_s* handle);

/**
 * @brief 获取存活的线程的个数
 *
 * @param handle 句柄
 * @return 返回活着的线程的个数
 */
hy_s32_t HyThreadPoolDynamicGetAliveNum(HyThreadPoolDynamic_s* handle);

#ifdef __cplusplus
}
#endif

#endif

