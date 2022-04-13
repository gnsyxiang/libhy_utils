/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_utils_linux.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/03 2022 19:13
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/03 2022      create the file
 * 
 *     last modified: 30/03 2022 19:13
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_UTILS_LINUX_H_
#define __LIBHY_UTILS_INCLUDE_HY_UTILS_LINUX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "hy_hal/hy_type.h"

/**
 * @brief 生成随机数
 *
 * @param range 限定随机数的范围
 *
 * @return 生成1到range之间的任意一个数
 */
hy_u32_t HyUtilsLinuxRandomNum(hy_u32_t range);

/**
 * @brief 执行命令
 *
 * @param _cmd 命令
 *
 * @return 无
 */
#define HyUtilsSystemCmd_m(_cmd, _ret)                      \
    ({                                                      \
        hy_s32_t ret = 0xffffffff;                          \
        do {                                                \
            if (HY_STRLEN(_cmd) <= 0) {                     \
                break;                                      \
            }                                               \
            ret = system(_cmd);                             \
            if (_ret == ret) {                              \
                LOGI("system cmd: %s \n", _cmd);            \
                break;                                      \
            }                                               \
            LOGE("system cmd failed, cmd: %s \n", _cmd);    \
        } while (0);                                        \
        (ret == _ret) ? 0 : -1;                             \
     })

#ifdef __cplusplus
}
#endif

#endif

