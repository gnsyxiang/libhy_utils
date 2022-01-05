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

typedef enum {
    HY_THREAD_POOL_OK               = 0,
    HY_THREAD_POOL_INVALID          = -1,
    HY_THREAD_POOL_TASK_FULL        = -2,
    HY_THREAD_POOL_SHUT_DOWN        = -3,
    HY_THREAD_POOL_SYSCALL_FAILD    = -4,
} HyThreadPoolError_t;

typedef enum {
    HY_THREAD_POOL_DESTROY_RUNNING,
    HY_THREAD_POOL_DESTROY_IMMEDIATE,
    HY_THREAD_POOL_DESTROY_GRACEFUL,
} HyThreadPoolDestroyFlag_t;

typedef void (*HyThreadPoolTaskCb_t)(void *args);

typedef struct {
    hy_u32_t task_max_cnt;
    hy_u32_t thread_max_cnt;

    HyThreadPoolDestroyFlag_t shutdown_flag;
} HyThreadPoolConfig_t;

void *HyThreadPoolCreate(HyThreadPoolConfig_t *config);
void HyThreadPoolDestroy(void **handle);

hy_s32_t HyThreadPoolAdd(void *handle, HyThreadPoolTaskCb_t task_cb, void *args);

#ifdef __cplusplus
}
#endif

#endif
