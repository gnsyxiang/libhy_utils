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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_string.h"
#include "hy_mem.h"
#include "hy_utils.h"

#include "cjson_impl.h"
#include "hy_json.h"

#if (HY_JSON_USE_TYPE == 1)

#define _get_item_com(_root, _n, _item)                         \
    do {                                                        \
        if (!_root) {                                           \
            LOGE("the root is NULL \n");                        \
            break;                                              \
        }                                                       \
        va_list list;                                           \
        va_start(list, _n);                                     \
        _item = _get_item_va_list(_root, n, list);              \
        va_end(list);                                           \
    } while (0);

#define _get_item(_error_val, _root, _n, _type, _cb)            \
    ({                                                          \
        void *item = NULL;                                      \
        _get_item_com(_root, _n, item);                         \
        if (item && _type == json_impl.item_typeof(item)) {     \
            _error_val = _cb(item);                             \
        }                                                       \
        _error_val;                                             \
     })

#define _set_item(_val, _root, _n, _type, _cb)                  \
    ({                                                          \
        void *item = NULL;                                      \
        hy_s32_t ret = -1;                                      \
        _get_item_com(_root, _n, item);                         \
        if (item && _type == json_impl.item_typeof(item)) {     \
            ret = _cb(item, _val);                              \
        }                                                       \
        ret;                                                    \
     })

static void *_get_item_va_list(void *root, hy_s32_t n, va_list list)
{
    void *item = root;
    HyJsonType_t type;

    for (hy_s32_t i = 0; i < n; i++) {
        hy_s32_t index;
        const char *str;
        type = json_impl.item_typeof(item);

        if (type == HY_JSON_ARRAY) {
            index = va_arg(list, hy_s32_t);
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

hy_s32_t HyJsonOperItemInt_va(hy_s32_t type, hy_s32_t val,
        void *root, hy_s32_t n, ...)
{
    if (type == 0) {
        return _get_item(val, root, n, HY_JSON_REAL, json_impl.item_get_int);
    } else {
        return _set_item(val, root, n, HY_JSON_REAL, json_impl.item_set_int);
    }
}

double HyJsonOperItemReal_va(hy_s32_t type, double val,
        void *root, hy_s32_t n, ...)
{
    if (type == 0) {
        return _get_item(val, root, n, HY_JSON_REAL, json_impl.item_get_real);
    } else {
        return _set_item(val, root, n, HY_JSON_REAL, json_impl.item_set_real);
    }
}

const char *HyJsonGetItemStr_va(const char *val, void *root, hy_s32_t n, ...)
{
    return _get_item(val, root, n, HY_JSON_STR, json_impl.item_get_str);
}

hy_s32_t HyJsonSetItemStr_va(const char *val, void *root, hy_s32_t n, ...)
{
    return _set_item(val, root, n, HY_JSON_STR, json_impl.item_set_str);
}

#endif

#if (HY_JSON_USE_TYPE == 2)

static hy_s32_t _get_index(char *fmt)
{
    hy_s32_t index;

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

static void *_get_item(void *root, char *fmt, size_t fmt_len)
{
    size_t i;
    size_t offset = 0;
    hy_s32_t index = 0;
    void *child = NULL;
    void *parent = root;
    hy_s32_t fmt_len_tmp = (hy_s32_t)fmt_len;

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

static void *_get_item_com(void *root, const char *fmt, size_t fmt_len)
{
    void *item = NULL;
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

hy_s32_t HyJsonGetItemInt2(hy_s32_t error_val, void *root, char *fmt, size_t fmt_len)
{
    void *item = _get_item_com(root, fmt, fmt_len);

    return (item != NULL) ? json_impl.item_to_int(item) : error_val;
}

double HyJsonGetItemReal2(double error_val,
        void *root, char *fmt, size_t fmt_len)
{
    void *item = _get_item_com(root, fmt, fmt_len);

    return (item != NULL) ? json_impl.item_to_real(item) : error_val;
}

const char *HyJsonGetItemStr2(const char *error_val,
        void *root, char *fmt, size_t fmt_len)
{
    void *item = _get_item_com(root, fmt, fmt_len);

    return (item != NULL) ? json_impl.item_to_str(item) : error_val;
}

#endif

static inline void _file_content_destroy(char **buf)
{
    HY_MEM_FREE_PP(buf);
}

static size_t _file_content_create(const char *file, char **buf)
{
    hy_s32_t fd;
    off_t offset = 0;

    do {
        if (0 != access(file, 0)) {
            LOGES("the %s file not exist \n", file);
            break;
        }

        fd = open(file, O_RDONLY, 0644);
        if (fd < 0) {
            LOGES("open %s file failed \n", file);
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

static size_t _save_file_content(HyJsonFile_s *json_file)
{
    hy_s32_t fd;
    hy_s32_t ret;
    off_t offset = 0;
    char *buf = json_impl.item_print_str(json_file->root);
    char file[1024] = {0};

    snprintf(file, 1024, "%s-bak", json_file->file);

    do {
        fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            LOGES("open %s file failed \n", file);
            break;
        }

        if (-1 == write(fd, buf, strlen(buf))) {
            LOGE("write failed \n");
            break;
        }

        HY_MEMSET(file, 1024);
        snprintf(file, 1024, "mv %s-bak %s", json_file->file, json_file->file);
        ret = HyUtilsSystemCmd_m(file, 0);
        if (0 != ret) {
            return -1;
        }

        close(fd);

        return offset;
    } while (0);

    if (fd) {
        close(fd);
    }

    return 0;
}

void HyJsonFileDestroy(HyJsonFile_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyJsonFile_s *json_file = *handle_pp;

    _save_file_content(json_file);

    json_impl.item_destroy(json_file->root);

    HY_MEM_FREE_PP(handle_pp);
}

HyJsonFile_s *HyJsonFileCreate(const char *file)
{
    HY_ASSERT_RET_VAL(!file, NULL);
    size_t len;
    char *buf = NULL;
    HyJsonFile_s *json_file = NULL;

    json_file = HY_MEM_CALLOC_RETURN_VAL(HyJsonFile_s *, sizeof(*json_file), NULL);
    json_file->file = file;

    len = _file_content_create(file, &buf);
    if (len > 0) {
        json_file->root = json_impl.item_create(buf);
        _file_content_destroy((char **)&buf);
    }

    return json_file;
}

char *HyJsonDump(void *root)
{
    HY_ASSERT_RET_VAL(!root, NULL);

    return json_impl.item_print_str(root);
}

void HyJsonDestroy(void *root)
{
    HY_ASSERT_RET(!root);

    json_impl.item_destroy(root);
}

void *HyJsonCreate(const char *buf)
{
    HY_ASSERT_RET_VAL(!buf, NULL);

    void *root = json_impl.item_create(buf);
    if (!root) {
        LOGE("create json failed \n");
    }

    return root;
}

