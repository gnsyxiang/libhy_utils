/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_cjson.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 19:39
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 19:39
 */
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "hy_cjson.h"

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_log.h"

#define ALONE_DEBUG 1

#if (HY_CJSON_USE_TYPE == 1)

#define _get_a_item(root, n, item, _type)           \
    do {                                            \
        if (!root) {                                \
            LOGE("the root is NULL \n");            \
            item = NULL;                            \
            break;                                  \
        }                                           \
        va_list list;                               \
        \
        va_start(list, n);                          \
        item = _get_item_va_list(root, n, list);    \
        va_end(list);                               \
        \
        if (_type != item->type) {                  \
            item = NULL;                            \
        }                                           \
    } while (0);

static cJSON *_get_item_va_list(cJSON *root, int n, va_list list)
{
    cJSON *item = root;

    for (int i = 0; i < n; i++) {
        int index;
        const char *str;

        if (item->type == cJSON_Array) {
            index = va_arg(list, int);
            item = cJSON_GetArrayItem(item, index);
        } else {
            str = va_arg(list, const char *);
            item = cJSON_GetObjectItem(item, str);
        }

        if (item == NULL) {
            break;
        }
    }

    return item;
}

int HyCjsonGetItemInt_va(int error_val, cJSON *root, int n, ...)
{
    cJSON *item;
    _get_a_item(root, n, item, cJSON_Number);

    return (item != NULL) ? item->valueint : error_val;
}

double HyCjsonGetItemDouble_va(double error_val, cJSON *root, int n, ...)
{
    cJSON *item;
    _get_a_item(root, n, item, cJSON_Number);

    return (item != NULL) ? item->valuedouble : error_val;
}

const char *HyCjsonGetItemStr_va(const char *error_val, cJSON *root, int n, ...)
{
    cJSON *item;
    _get_a_item(root, n, item, cJSON_String);

    return (item != NULL) ? item->valuestring : error_val;
}
#endif

#if (HY_CJSON_USE_TYPE == 2)

static int32_t _get_index(char *fmt)
{
    int32_t index;

    char *start = strstr(fmt, "[");
    if (NULL != start) {
        char *end = strstr(fmt, "]");
        if (NULL != end) {
            *start = *end = '\0';
            start++;
            sscanf(start, "%d", &index);
            if (index >= 0) {
                return index;
            }
        }
    }

    return -1;
}

static cJSON *_get_item(cJSON *root, char *fmt, size_t fmt_len)
{
    size_t i;
    size_t offset = 0;
    int32_t index = 0;
    cJSON *child = NULL;
    cJSON *parent = root;
    int32_t fmt_len_tmp = (int32_t)fmt_len;

    for (i = 0; i < fmt_len; ++i) {
        if ('.' == fmt[i]) {
            fmt[i] = '\0';
        }
    }

    while (fmt_len_tmp > 0) {
        offset = HY_STRLEN(fmt) + 1;
        index = _get_index(fmt);

        child = cJSON_GetObjectItem(parent, fmt);
        if (!child) {
            break;
        }

        if (index >= 0) {
            child = cJSON_GetArrayItem(child, index);
            if (!child) {
                break;
            }
        }

        parent       = child;
        fmt         += offset;
        fmt_len_tmp -= offset;
    }

    return child;
}

static cJSON *_get_item_com(cJSON *root, const char *fmt, size_t fmt_len)
{
    cJSON *item = NULL;
    char *cp_fmt, *cp_fmt_tmp;
    size_t len;

    len = HY_MEM_ALIGN4_UP(fmt_len);
    cp_fmt = HY_MEM_MALLOC_RET_VAL(char *, len, NULL);
    cp_fmt_tmp = cp_fmt;

    HY_MEMSET(cp_fmt, len);
    HY_MEMCPY(cp_fmt, fmt, fmt_len);

    item = _get_item(root, cp_fmt, fmt_len);

    HY_MEM_FREE_PP((void **)&cp_fmt_tmp);

    return item;
}

int HyCjsonGetItemInt2(int error_val, cJSON *root, char *fmt, size_t fmt_len)
{
    cJSON *item = _get_item_com(root, fmt, fmt_len);

    return (item != NULL) ? item->valueint : error_val;
}

double HyCjsonGetItemDouble2(double error_val,
        cJSON *root, char *fmt, size_t fmt_len)
{
    cJSON *item = _get_item_com(root, fmt, fmt_len);

    return (item != NULL) ? item->valuedouble : error_val;
}

const char *HyCjsonGetItemStr2(const char *error_val,
        cJSON *root, char *fmt, size_t fmt_len)
{
    cJSON *item = _get_item_com(root, fmt, fmt_len);

    return (item != NULL) ? item->valuestring : error_val;
}

#endif

static inline void _file_content_destroy(char **buf)
{
    HY_MEM_FREE_PP(buf);
}

static char *_file_content_create(const char *name)
{
    int fd;
    off_t offset = 0;
    char *buf = NULL;

    do {
        if (0 != access(name, 0)) {
            LOGE("the %s file not exist \n", name);
            break;
        }

        fd = open(name, O_RDONLY, 0644);
        if (fd < 0) {
            LOGE("open %s file failed \n", name);
            break;
        }

        offset = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);

        buf = HY_MEM_MALLOC_BREAK(char *, offset + 1);

        if (offset != read(fd, buf, offset)) {
            LOGE("read len failed \n");
            break;
        }

        close(fd);

        return buf;
    } while (0);

    _file_content_destroy((char **)&buf);
    return NULL;
}

void HyCjsonFileParseDestroy(cJSON *root)
{
    cJSON_Delete(root);
}

cJSON *HyCjsonFileParseCreate(const char *name)
{
    char *buf = NULL;
    cJSON *root = NULL;

    buf = _file_content_create(name);
    root = cJSON_Parse(buf);
    _file_content_destroy((char **)&buf);

    return root;
}
