/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_id.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/02 2022 10:07
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/02 2022      create the file
 * 
 *     last modified: 17/02 2022 10:07
 */
#ifndef __LIBHY_UTILS_INCLUDE_IPC_PROCESS_ID_H_
#define __LIBHY_UTILS_INCLUDE_IPC_PROCESS_ID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

/**
 * @brief 通信id
 */
typedef enum {
    HY_IPC_PROCESS_ID_TEST,

    HY_IPC_PROCESS_ID_MAX,
} HyIpcProcessId_e;

// HY_IPC_PROCESS_ID_TEST
typedef struct {
    hy_s32_t test;
} HyIpcProcessTestId_s;

#ifdef __cplusplus
}
#endif

#endif
