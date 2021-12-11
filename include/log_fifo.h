/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    log_fifo.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    11/12 2021 20:56
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        11/12 2021      create the file
 * 
 *     last modified: 11/12 2021 20:56
 */
#ifndef __LIBHY_UTILS_INCLUDE_LOG_FIFO_H_
#define __LIBHY_UTILS_INCLUDE_LOG_FIFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

/**
 * @brief 锁状态
 */
typedef enum {
    LOG_FIFO_MUTEX_UNLOCK,               ///< 不加锁
    LOG_FIFO_MUTEX_LOCK,                 ///< 加锁
} log_fifo_mutex_flag_t;

void *log_fifo_create(hy_u32_t len);
void log_fifo_destroy(void **handle);
hy_s32_t log_fifo_is_empty(void *handle);

hy_u32_t log_fifo_read(void *handle, void *buf, hy_u32_t len);
hy_u32_t log_fifo_write(void *handle, void *buf, hy_u32_t len);

#ifdef __cplusplus
}
#endif

#endif

