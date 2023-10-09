/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_dynamic_lib.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    09/10 2023 14:05
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        09/10 2023      create the file
 * 
 *     last modified: 09/10 2023 14:05
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_DYNAMIC_LIB_H_
#define __LIBHY_UTILS_INCLUDE_HY_DYNAMIC_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

void *HyDynamicLibOpen(const char *lib_name);
void HyDynamicLibClose(void **handle_pp);

void *HyDynamicLibLoadSymbol(void *handle, const char *symbol);

#ifdef __cplusplus
}
#endif

#endif

