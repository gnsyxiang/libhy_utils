/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    thread_specific_data.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    28/04 2022 10:38
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        28/04 2022      create the file
 * 
 *     last modified: 28/04 2022 10:38
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_THREAD_SPECIFIC_DATA_H_
#define __LIBHY_UTILS_INCLUDE_HY_THREAD_SPECIFIC_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include <hy_log/hy_type.h>

typedef void * (*HyThreadSpecificDataCreateCb_t)(void);
typedef void (*HyThreadSpecificDataDestroyCb_t)(void *handle);

typedef void (*HyThreadSpecificDataResetCb_t)(void *handle);

typedef struct {
    pthread_key_t                       thread_key;

    HyThreadSpecificDataCreateCb_t      create_cb;
    HyThreadSpecificDataDestroyCb_t     destroy_cb;
    HyThreadSpecificDataResetCb_t       reset_cb;
} HyThreadSpecificData_s;

hy_s32_t HyThreadSpecificDataCreate(
    HyThreadSpecificDataCreateCb_t create_cb,
    HyThreadSpecificDataDestroyCb_t destroy_cb,
    HyThreadSpecificDataResetCb_t reset_cb);

void HyThreadSpecificDataDestroy(void);

void *HyThreadSpecificDataFetch(void);

#ifdef __cplusplus
}
#endif

#endif

