/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_video.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    09/03 2022 14:11
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        09/03 2022      create the file
 * 
 *     last modified: 09/03 2022 14:11
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_VIDEO_H_
#define __LIBHY_UTILS_INCLUDE_HY_VIDEO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

/**
 * @brief 视频尺寸
 *
 * @note
 * 144P  = 192  × 144
 * 240P  = 320  x 240
 * 360P  = 480  x 360
 * 480P  = 640  × 480
 * 720P  = 1280 × 720
 * 1080P = 1920 × 1080
 */
typedef struct {
    hy_u32_t width;
    hy_u32_t height;
} HyVideoSize_s;

#ifdef __cplusplus
}
#endif

#endif

