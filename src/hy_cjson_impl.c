/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_cjson_impl.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/11 2021 10:50
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/11 2021      create the file
 * 
 *     last modified: 03/11 2021 10:50
 */
#include <stdio.h>

#include <cjson/cJSON.h>

#include "hy_cjson_impl.h"

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_string.h"

#include "hy_log.h"

HyJson_t *_cjson_item_create(const char *buf, size_t len)
{
    return cJSON_Parse(buf);
}

void _cjson_item_destroy(HyJson_t *root)
{
    cJSON_Delete((cJSON*)root);
}

HyJson_t *_cjson_item_new(void)
{
    return cJSON_CreateObject();
}

HyJson_t *_cjson_item_get(const HyJson_t *root, const char *key)
{
    return cJSON_GetObjectItem((cJSON*)root, key);
}

int _cjson_item_add(HyJson_t *root, const char *field, HyJson_t *item)
{
    cJSON_AddItemToObject((cJSON*)root, field, (cJSON*)item);
    return 0;
}

char _cjson_item_to_bool(const HyJson_t *item)
{
    return ((cJSON*)item)->type == cJSON_True;
}

long long _cjson_item_to_int(const HyJson_t *item)
{
    return ((cJSON*)item)->valueint;
}

double _cjson_item_to_real(const HyJson_t *item)
{
    return ((cJSON*)item)->valuedouble;
}

const char *_cjson_item_to_str(const HyJson_t *item)
{
    return cJSON_GetStringValue((cJSON*)item);
}

size_t _cjson_item_to_str_len(const HyJson_t *item)
{
    return strlen(cJSON_GetStringValue((cJSON*)item));
}

HyJson_t *_cjson_item_from_bool(char val)
{
    return cJSON_CreateBool(val);
}

HyJson_t *_cjson_item_from_int(long long val)
{
    cJSON* tmp = cJSON_CreateNumber(0);
    cJSON_SetNumberValue(tmp, val);
    return tmp;
}

HyJson_t *_cjson_item_from_real(double val)
{
    return cJSON_CreateNumber(val);
}

HyJson_t *_cjson_item_from_str(const char *val)
{
    return cJSON_CreateString(val);
}

HyJson_t *_cjson_item_array_new(void)
{
    return cJSON_CreateArray();
}

int _cjson_item_array_add(HyJson_t *array, HyJson_t *item)
{
    cJSON_AddItemToArray((cJSON*)array, (cJSON*)item);
    return 0;
}

HyJson_t *_cjson_item_array_get(const HyJson_t *array, size_t index)
{
    return cJSON_GetArrayItem((cJSON*)array, index);
}

size_t _cjson_item_array_size(const HyJson_t *array)
{
    return cJSON_GetArraySize((cJSON*)array);
}

HyJsonType_t _cjson_item_typeof(const HyJson_t *item)
{
    switch(((cJSON*)item)->type){
        case cJSON_Invalid:
        case cJSON_NULL:
            return HY_JSON_NULL;
        case cJSON_False:
            return HY_JSON_FALSE;
        case cJSON_True:
            return HY_JSON_TRUE;
        case cJSON_Number:
            return HY_JSON_REAL;
        case cJSON_String:
        case cJSON_Raw:
            return HY_JSON_STR;
        case cJSON_Array:
            return HY_JSON_ARRAY;
        case cJSON_Object:
           return HY_JSON_OBJECT;
        default:
            return HY_JSON_NULL;
    }
}

char *_cjson_item_print_str(const HyJson_t *root)
{
    return cJSON_Print((cJSON*)root);
}

json_impl_t json_impl = {
    .item_create        = _cjson_item_create,
    .item_destroy       = _cjson_item_destroy,

    .item_new           = _cjson_item_new,
    .item_get           = _cjson_item_get,
    .item_add           = _cjson_item_add,

    .item_to_bool       = _cjson_item_to_bool,
    .item_to_int        = _cjson_item_to_int,
    .item_to_real       = _cjson_item_to_real,
    .item_to_str        = _cjson_item_to_str,
    .item_to_str_len    = _cjson_item_to_str_len,

    .item_from_bool     = _cjson_item_from_bool,
    .item_from_int      = _cjson_item_from_int,
    .item_from_real     = _cjson_item_from_real,
    .item_from_str      = _cjson_item_from_str,

    .item_array_new     = _cjson_item_array_new,
    .item_array_add     = _cjson_item_array_add,
    .item_array_get     = _cjson_item_array_get,
    .item_array_size    = _cjson_item_array_size,

    .item_typeof        = _cjson_item_typeof,
    .item_print_str     = _cjson_item_print_str,
};

