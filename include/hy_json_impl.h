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

#include "hy_json.h"

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
    HyJson_t *(*item_create)(const char *buf, size_t len);
    void (*item_destroy)(HyJson_t *root);

    HyJson_t *(*item_new)(void);
    HyJson_t *(*item_get)(const HyJson_t *root, const char *key);
    int (*item_add)(HyJson_t *root, const char *field, HyJson_t *item);

    char (*item_to_bool)(const HyJson_t *item);
    long long (*item_to_int)(const HyJson_t *item);
    double (*item_to_real)(const HyJson_t *item);
    const char *(*item_to_str)(const HyJson_t *item);
    size_t (*item_to_str_len)(const HyJson_t *item);

    HyJson_t *(*item_from_bool)(char val);
    HyJson_t *(*item_from_int)(long long val);
    HyJson_t *(*item_from_real)(double val);
    HyJson_t *(*item_from_str)(const char *val);

    HyJson_t *(*item_array_new)(void);
    int (*item_array_add)(HyJson_t *array, HyJson_t *item);
    HyJson_t *(*item_array_get)(const HyJson_t *array, size_t index);
    size_t (*item_array_size)(const HyJson_t *array);

    HyJsonType_t (*item_typeof)(const HyJson_t *item);
    char *(*item_print_str)(const HyJson_t *root);
} json_impl_t;

#ifdef __cplusplus
}
#endif

#endif

