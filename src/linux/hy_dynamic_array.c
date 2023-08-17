/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    dynamic_array.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    22/04 2022 09:06
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        22/04 2022      create the file
 * 
 *     last modified: 22/04 2022 09:06
 */
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include <hy_log/hy_log.h>

#include "hy_dynamic_array.h"
#include "hy_assert.h"
#include "hy_mem.h"

/**
 * @brief 动态数组
 *
 * @note 空间不够时，自动开辟空间，直到开辟到最大的空间
 */
struct HyDynamicArray_s {
    char        *buf;       ///< 存放数据的地方
    hy_u32_t    len;        ///< 开辟空间的大小
    hy_u32_t    cur_len;    ///< 已经存储数据的大小

    hy_u32_t    write_pos;  ///< 写位置
    hy_u32_t    read_pos;   ///< 读位置

    hy_u32_t    min_len;    ///< 开辟空间的最小长度
    hy_u32_t    max_len;    ///< 开辟空间的最大长度
};

hy_s32_t HyDynamicArrayRead(HyDynamicArray_s *handle, void *buf, hy_u32_t len)
{
    assert(handle);
    assert(buf);

    if (len > handle->cur_len) {
        len = handle->cur_len;
    }

    memcpy(buf, handle->buf + handle->read_pos, len);
    handle->read_pos += len;
    handle->cur_len  -= len;

    return len;
}

static hy_s32_t _dynamic_array_extend(HyDynamicArray_s *handle, hy_u32_t increment)
{
    assert(handle);
    hy_u32_t extend_len = 0;
    hy_s32_t ret = 0;
    void *ptr = NULL;

    if (handle->len >= handle->max_len) {
        LOGE("can't extend size, len: %d, max_len: %d \n",
             handle->len, handle->max_len);
        return -1;
    }

    if (handle->len + increment <= handle->max_len) {
        extend_len = HY_MEM_ALIGN4_UP(handle->len + increment + 1);
    } else {
        extend_len = handle->max_len;
        ret = 1;
        LOGE("extend to max len, len: %d, extend_len: %d \n",
             handle->len, extend_len);
    }

    ptr = realloc(handle->buf, extend_len);
    if (!ptr) {
        LOGE("realloc failed \n");
        return -1;
    }

    handle->buf = ptr;
    handle->len = extend_len;

    return ret;
}

hy_s32_t HyDynamicArrayWriteVprintf(HyDynamicArray_s *handle,
                                    const char *format, va_list *args)
{
    assert(handle);
    assert(format);
    assert(args);
#define _get_dynamic_info(_handle)                      \
do {                                                    \
    va_copy(ap, *args);                                 \
    free_len = _handle->len - _handle->cur_len - 1;     \
    ptr = _handle->buf + _handle->write_pos;            \
} while (0)

    va_list ap;
    hy_s32_t free_len;
    hy_s32_t ret;
    char *ptr = NULL;

    _get_dynamic_info(handle);
    ret = vsnprintf(ptr, free_len, format, ap);
    if (ret < 0) {
        LOGE("vsnprintf failed \n");
        ret = -1;
    } else if (ret >= 0) {
        if (ret < free_len) {
            handle->cur_len += ret;
            handle->write_pos += ret;
        } else {
            ret = _dynamic_array_extend(handle,
                                        ret - (handle->len - handle->cur_len));
            if (-1 == ret) {
                LOGI("_dynamic_array_extend failed \n");
            } else if (ret >= 0) {
                _get_dynamic_info(handle);
                ret = vsnprintf(ptr, free_len, format, ap);
                handle->cur_len += ret;
                handle->write_pos += ret;

                /* FIXME: <22-04-23, uos> 做进一步判断 */
            }
        }
    }

    return ret;
}

hy_s32_t HyDynamicArrayWrite(HyDynamicArray_s *handle, const void *buf, hy_u32_t len)
{
#define _write_data_com(_handle, _buf, _len)    \
do {                                            \
    char *ptr = NULL;                           \
    ptr = _handle->buf + _handle->write_pos;    \
    memcpy(ptr, _buf, _len);                    \
    _handle->cur_len      += _len;              \
    _handle->write_pos    += _len;              \
} while (0)

    assert(handle);
    assert(buf);

    hy_s32_t ret = 0;

    if (handle->len - handle->cur_len > len) {
        _write_data_com(handle, buf, len);
    } else {
        ret = _dynamic_array_extend(handle,
                                    len - (handle->len - handle->cur_len));
        if (-1 == ret) {
            LOGI("_dynamic_array_extend failed \n");
            len = -1;
        } else if (0 == ret) {
            _write_data_com(handle, buf, len);
        } else {
            // 3 for "..." 1 for "\0"
            len = handle->len - handle->cur_len - 3 - 1;
            _write_data_com(handle, buf, len);

            LOGE("truncated data \n");
            _write_data_com(handle, "...", 3);
            len += 3;
        }
    }

    return len;
}

void HyDynamicArrayReset(HyDynamicArray_s *handle)
{
    HY_ASSERT_RET(!handle);

    HY_MEMSET(handle->buf, handle->len);
    handle->cur_len = 0;
    handle->write_pos = 0;
    handle->read_pos = 0;
}

void HyDynamicArrayDestroy(HyDynamicArray_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyDynamicArray_s *handle = *handle_pp;

    HY_MEM_FREE_PP(&handle->buf);

    LOGI("dynamic array destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyDynamicArray_s *HyDynamicArrayCreate(hy_u32_t min_len, hy_u32_t max_len)
{
    HyDynamicArray_s *handle = NULL;
    do {
        handle = HY_MEM_CALLOC_BREAK(HyDynamicArray_s *, sizeof(*handle));
        handle->buf = HY_MEM_CALLOC_BREAK(char *, min_len);

        handle->max_len    = max_len;
        handle->min_len    = min_len;
        handle->len        = handle->min_len;
        handle->write_pos  = handle->read_pos = 0;

        LOGI("dynamic array create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("dynamic array create failed \n");
    HyDynamicArrayDestroy(&handle);
    return NULL;
}
