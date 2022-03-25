/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_json_impl.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/11 2021 09:52
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/11 2021      create the file
 * 
 *     last modified: 03/11 2021 09:52
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_JSON_IMPL_H_
#define __LIBHY_UTILS_INCLUDE_HY_JSON_IMPL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "hy_hal/hy_type.h"

typedef enum {
    HY_JSON_OBJECT,
    HY_JSON_ARRAY,
    HY_JSON_STR,
    HY_JSON_INT,
    HY_JSON_REAL,
    HY_JSON_TRUE,
    HY_JSON_FALSE,
    HY_JSON_NULL
} HyJsonType_t;

typedef struct {
    void *(*item_create)(const char *buf);
    void (*item_destroy)(void *root);

    void *(*item_new)(void);
    void *(*item_get)(const void *root, const char *key);
    int (*item_add)(void *root, const char *field, void *item);

    char (*item_get_bool)(const void *item);
    long long (*item_get_int)(const void *item);
    double (*item_get_real)(const void *item);
    const char *(*item_get_str)(const void *item);

    hy_s32_t (*item_set_bool)(const void *item, char val);
    hy_s32_t (*item_set_int)(const void *item, hy_s32_t val);
    hy_s32_t (*item_set_real)(const void *item, double val);
    hy_s32_t (*item_set_str)(const void *item, const char *val);

    void *(*item_from_bool)(char val);
    void *(*item_from_int)(long long val);
    void *(*item_from_real)(double val);
    void *(*item_from_str)(const char *val);

    void *(*item_array_new)(void);
    int (*item_array_add)(void *array, void *item);
    void *(*item_array_get)(const void *array, size_t index);
    size_t (*item_array_size)(const void *array);

    HyJsonType_t (*item_typeof)(const void *item);
    char *(*item_print_str)(const void *root);
} json_impl_t;

#ifdef __cplusplus
}
#endif

#endif

