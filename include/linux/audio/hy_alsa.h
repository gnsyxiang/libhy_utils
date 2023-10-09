/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_alsa.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/07 2021 16:37
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/07 2021      create the file
 * 
 *     last modified: 30/07 2021 16:37
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_ALSA_H_
#define __LIBHY_UTILS_INCLUDE_HY_ALSA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include <alsa/asoundlib.h>

#include "hy_utils/hy_type.h"

typedef enum {
    HY_ALSA_OPT_PLAY,
    HY_ALSA_OPT_RECORD,

    HY_ALSA_OPT_MAX
} HyAlsaOptType_t;

typedef struct {
    const char *snd_dev_name;
    const char *format_name;
    hy_u32_t channels;
    hy_u32_t sample_rate;

    hy_u32_t period_time;
} HyAlsaSaveConfig_t;

typedef struct {
    HyAlsaSaveConfig_t save_config;
    HyAlsaOptType_t type;
} HyAlsaConfig_t;

void *HyAlsaCreate(HyAlsaConfig_t *alsa_config);
void HyAlsaDestroy(void **handle);

hy_s32_t HyAlsaGetData(void *handle, void *data);
void HyAlsaPutData(void *handle, void *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif

