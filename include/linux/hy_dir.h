/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_dir.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    09/10 2023 09:12
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        09/10 2023      create the file
 * 
 *     last modified: 09/10 2023 09:12
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_DIR_H_
#define __LIBHY_UTILS_INCLUDE_HY_DIR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_os_type/hy_os_type.h>

typedef enum HyDirType_e {
    HY_DIR_TYPE_FILE,
    HY_DIR_TYPE_DIR,
} HyDirType_e;

typedef enum HyDirRecurse_e {
    HY_DIR_RECURSE_OPEN,
    HY_DIR_RECURSE_CLOSE,
} HyDirRecurse_e;

typedef void (*HyDirReadDirCb_t)(const char *path, const char *name,
                                 HyDirType_e type, void *args);

// @param filter 过滤后缀，NULL为不过滤
hy_s32_t HyDirRead(const char *path, const char *filter, hy_s32_t recurse_flag,
                   HyDirReadDirCb_t read_dir_cb, void *args);

#ifdef __cplusplus
}
#endif

#endif

