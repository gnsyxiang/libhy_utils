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
#include "hy_hal/hy_compile.h"

#define HY_IPC_PROCESS_MSG_ID_SYNC_BASE         0x0000
#define HY_IPC_PROCESS_MSG_ID_BROADCAST_BASE    0x8000

/**
 * @brief 通信id
 *
 * @note 广播消息需要设置在HY_IPC_PROCESS_MSG_ID_BROADCAST_BASE之后
 */
typedef enum {
    HY_IPC_PROCESS_MSG_ID_SYNC_DEMO_PARAM_GET = HY_IPC_PROCESS_MSG_ID_SYNC_BASE,
    HY_IPC_PROCESS_MSG_ID_SYNC_DEMO_PARAM_SET,

    HY_IPC_PROCESS_MSG_ID_BROADCAST_DEMO_PARAM = HY_IPC_PROCESS_MSG_ID_BROADCAST_BASE,

    HY_IPC_PROCESS_MSG_ID_MAX,
} HyIpcProcessMsgId_e;

typedef struct {
    HyIpcProcessMsgId_e     id;

    // send

    // recv
    char                    buf[4];
} UNPACKED DemoParamGet_s;

typedef struct {
    HyIpcProcessMsgId_e     id;

    // send
    char                    buf[4];

    // recv
} UNPACKED DemoParamSet_s;

#ifdef __cplusplus
}
#endif

#endif
