/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process_id.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    02/03 2022 16:58
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        02/03 2022      create the file
 * 
 *     last modified: 02/03 2022 16:58
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_IPC_PROCESS_ID_H_
#define __LIBHY_UTILS_INCLUDE_HY_IPC_PROCESS_ID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

#define HY_IPC_PROCESS_MSG_ID_SYNC_START        (0x0000)
#define HY_IPC_PROCESS_MSG_ID_BROADCAST_START   (0x8000)

typedef enum {
    // sync data
    HY_IPC_PROCESS_MSG_ID_SYNC_BASE = HY_IPC_PROCESS_MSG_ID_SYNC_START,

    HY_IPC_PROCESS_MSG_ID_SYNC_AUDIO_PARAM_GET,
    HY_IPC_PROCESS_MSG_ID_SYNC_AUDIO_PARAM_SET,

    HY_IPC_PROCESS_MSG_ID_SYNC_VIDEO_PARAM_GET,
    HY_IPC_PROCESS_MSG_ID_SYNC_VIDEO_PARAM_SET,

    // broadcast
    HY_IPC_PROCESS_MSG_ID_BROADCAST_BASE = HY_IPC_PROCESS_MSG_ID_BROADCAST_START,

    HY_IPC_PROCESS_MSG_ID_BROADCAST_POWER_OFF,
} HyIpcProcessMsgId_e;

// HY_IPC_PROCESS_MSG_ID_SYNC_DATA_AUDIO_PARAM_GET
typedef struct {
    hy_s32_t type;
} HyIpcProcessAudioParamGet_s;

typedef struct {
    hy_u32_t channel;
    hy_u32_t sample_rate;
    hy_u32_t bit_per_sample;
} HyIpcProcessAudioParamGetResult_s;

// HY_IPC_PROCESS_MSG_ID_SYNC_DATA_AUDIO_PARAM_SET
typedef struct {
    hy_u32_t channel;
    hy_u32_t sample_rate;
    hy_u32_t bit_per_sample;
} HyIpcProcessAudioParamSet_s;

typedef struct {
    hy_u32_t reserved;
} HyIpcProcessAudioParamSetResult_s;

// HY_IPC_PROCESS_MSG_ID_SYNC_VIDEO_PARAM_GET
typedef struct {
    hy_u32_t reserved;
} HyIpcProcessVideoParamGet_s;

typedef struct {
    hy_u32_t width;
    hy_u32_t height;
} HyIpcProcessVideoParamGetResult_s;

// HY_IPC_PROCESS_MSG_ID_SYNC_VIDEO_PARAM_SET
typedef struct {
    hy_u32_t width;
    hy_u32_t height;
} HyIpcProcessVideoParamSet_s;

typedef struct {
    hy_u32_t reserved;
} HyIpcProcessVideoParamSetResult_s;

#ifdef __cplusplus
}
#endif

#endif

