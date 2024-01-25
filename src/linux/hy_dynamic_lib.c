/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_dynamic_lib.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    09/10 2023 14:11
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        09/10 2023      create the file
 * 
 *     last modified: 09/10 2023 14:11
 */
#include <stdio.h>
#include <dlfcn.h>

#include <hy_os_type/hy_assert.h>

#include "hy_dynamic_lib.h"

void *HyDynamicLibLoadSymbol(void *handle, const char *symbol)
{
    HY_ASSERT_RET_VAL(!handle || !symbol, NULL);

    void *symbol_handle = dlsym(handle, symbol);
    if (!symbol_handle) {
        LOGES("dlsym failed, %s \n", dlerror());
    }

    return symbol_handle;
}

void HyDynamicLibClose(void **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);

    dlclose(*handle_pp);

    *handle_pp = NULL;
}

void *HyDynamicLibOpen(const char *lib_name)
{
    HY_ASSERT_RET_VAL(!lib_name, NULL);

    void *handle = dlopen(lib_name, RTLD_LAZY);
    if (!handle) {
        LOGES("dlopen failed, error: %s \n", dlerror());
    }

    return handle;
}
