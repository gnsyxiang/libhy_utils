/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_json.c
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

#include "hy_json.h"
#include "hy_cjson_impl.h"

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_log.h"

#define ALONE_DEBUG 1

#if (HY_JSON_USE_TYPE == 1)

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
        if (_type != json_impl.item_typeof(item)) { \
            item = NULL;                            \
        }                                           \
    } while (0);

static HyJson_t *_get_item_va_list(HyJson_t *root, int n, va_list list)
{
    HyJson_t *item = root;
    HyJsonType_t type;

    for (int i = 0; i < n; i++) {
        int index;
        const char *str;
        type = json_impl.item_typeof(item);

        if (type == HY_JSON_ARRAY) {
            index = va_arg(list, int);
            item = json_impl.item_array_get(item, index);
        } else {
            str = va_arg(list, const char *);
            item = json_impl.item_get(item, str);
        }

        if (item == NULL) {
            break;
        }
    }

    return item;
}

int HyJsonGetItemInt_va(int error_val, HyJson_t *root, int n, ...)
{
    HyJson_t *item;
    _get_a_item(root, n, item, HY_JSON_REAL);

    return (item != NULL) ? json_impl.item_to_int(item) : error_val;
}

double HyJsonGetItemReal_va(double error_val, HyJson_t *root, int n, ...)
{
    HyJson_t *item;
    _get_a_item(root, n, item, HY_JSON_REAL);

    return (item != NULL) ? json_impl.item_to_real(item) : error_val;
}

const char *HyJsonGetItemStr_va(const char *error_val, HyJson_t *root, int n, ...)
{
    HyJson_t *item;
    _get_a_item(root, n, item, HY_JSON_STR);

    return (item != NULL) ? json_impl.item_to_str(item) : error_val;
}
#endif

#if (HY_JSON_USE_TYPE == 2)

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

static HyJson_t *_get_item(HyJson_t *root, char *fmt, size_t fmt_len)
{
    size_t i;
    size_t offset = 0;
    int32_t index = 0;
    HyJson_t *child = NULL;
    HyJson_t *parent = root;
    int32_t fmt_len_tmp = (int32_t)fmt_len;

    for (i = 0; i < fmt_len; ++i) {
        if ('.' == fmt[i]) {
            fmt[i] = '\0';
        }
    }

    while (fmt_len_tmp > 0) {
        offset = HY_STRLEN(fmt) + 1;
        index = _get_index(fmt);

        child = json_impl.item_get(parent, fmt);
        if (!child) {
            break;
        }

        if (index >= 0) {
            child = json_impl.item_array_get(child, index);
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

static HyJson_t *_get_item_com(HyJson_t *root, const char *fmt, size_t fmt_len)
{
    HyJson_t *item = NULL;
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

int HyJsonGetItemInt2(int error_val, HyJson_t *root, char *fmt, size_t fmt_len)
{
    HyJson_t *item = _get_item_com(root, fmt, fmt_len);

    return (item != NULL) ? json_impl.item_to_int(item) : error_val;
}

double HyJsonGetItemReal2(double error_val,
        HyJson_t *root, char *fmt, size_t fmt_len)
{
    HyJson_t *item = _get_item_com(root, fmt, fmt_len);

    return (item != NULL) ? json_impl.item_to_real(item) : error_val;
}

const char *HyJsonGetItemStr2(const char *error_val,
        HyJson_t *root, char *fmt, size_t fmt_len)
{
    HyJson_t *item = _get_item_com(root, fmt, fmt_len);

    return (item != NULL) ? json_impl.item_to_str(item) : error_val;
}

#endif

static inline void _file_content_destroy(char **buf)
{
    HY_MEM_FREE_PP(buf);
}

static size_t _file_content_create(const char *name, char **buf)
{
    int fd;
    off_t offset = 0;

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

        *buf = HY_MEM_MALLOC_BREAK(char *, offset + 1);

        if (offset != read(fd, *buf, offset)) {
            LOGE("read len failed \n");
            break;
        }

        close(fd);

        return offset;
    } while (0);

    _file_content_destroy(buf);
    return 0;
}

void HyJsonFileDestroy(HyJson_t *root)
{
    HY_ASSERT_VAL_RET(!root);
    json_impl.item_destroy(root);
}

HyJson_t *HyJsonFileCreate(const char *name)
{
    HY_ASSERT_VAL_RET_VAL(!name, NULL);

    size_t len;
    char *buf = NULL;
    HyJson_t *root = NULL;

    len = _file_content_create(name, &buf);
    if (len > 0) {
        root = json_impl.item_create(buf, len);
        _file_content_destroy((char **)&buf);
    }

    return root;
}

void HyJsonDump(HyJson_t *root)
{
    HY_ASSERT_VAL_RET(!root);

    char *buf = json_impl.item_print_str(root);
    LOGI("%s \n", buf);
    free(buf);
}

void HyJsonDestroy(HyJson_t *root)
{
    HY_ASSERT_VAL_RET(!root);

    json_impl.item_destroy(root);
}

HyJson_t *HyJsonCreate(const char *buf)
{
    HY_ASSERT_VAL_RET_VAL(!buf, NULL);

    HyJson_t *root = json_impl.item_create(buf, 0);
    if (!root) {
        LOGE("create json failed \n");
    }

    return root;
}
