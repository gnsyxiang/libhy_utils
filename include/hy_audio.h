/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_audio.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    07/03 2022 13:55
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        07/03 2022      create the file
 * 
 *     last modified: 07/03 2022 13:55
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_AUDIO_H_
#define __LIBHY_UTILS_INCLUDE_HY_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

typedef struct {
    hy_u32_t channel;
    hy_u32_t sample_rate;
    hy_u32_t bit_per_sample;
} HyAudioParam_s;

#ifdef __cplusplus
}
#endif

#endif

