/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_linux.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/08 2023 11:17
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/08 2023      create the file
 * 
 *     last modified: 17/08 2023 11:17
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_LINUX_H_
#define __LIBHY_UTILS_INCLUDE_HY_LINUX_H_

#ifdef __cplusplus
extern "C" {
#endif

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

