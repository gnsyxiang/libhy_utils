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

#define HY_MEM_ALIGN(len, align)    (((len) + (align) - 1) & ~((align) - 1))
#define HY_MEM_ALIGN4(len)          HY_MEM_ALIGN(len, 4)
#define HY_MEM_ALIGN4_UP(len)       (HY_MEM_ALIGN(len, 4) + HY_MEM_ALIGN4(1))

/**
 * @brief 动态数组
 *
 * @note 空间不够时，自动开辟空间，直到开辟到最大的空间
 */
typedef struct {
    char        *buf;       ///< 存放数据的地方
    hy_u32_t    len;        ///< 开辟空间的大小
    hy_u32_t    cur_len;    ///< 已经存储数据的大小

    hy_u32_t    write_pos;  ///< 写位置
    hy_u32_t    read_pos;   ///< 读位置

    hy_u32_t    min_len;    ///< 开辟空间的最小长度
    hy_u32_t    max_len;    ///< 开辟空间的最大长度
} dynamic_array_s;

hy_s32_t HyDynamicArrayRead(void *handle, void *buf, hy_u32_t len)
{
    assert(handle);
    assert(buf);

    dynamic_array_s *context = handle;

    if (len > context->cur_len) {
        len = context->cur_len;
    }

    memcpy(buf, context->buf + context->read_pos, len);
    context->read_pos += len;
    context->cur_len  -= len;

    return len;
}

static hy_s32_t _dynamic_array_extend(dynamic_array_s *context,
                                      hy_u32_t increment)
{
    assert(context);
    hy_u32_t extend_len = 0;
    hy_s32_t ret = 0;
    void *ptr = NULL;

    if (context->len >= context->max_len) {
        LOGE("can't extend size, len: %d, max_len: %d \n",
             context->len, context->max_len);
        return -1;
    }

    if (context->len + increment <= context->max_len) {
        extend_len = HY_MEM_ALIGN4_UP(context->len + increment + 1);
    } else {
        extend_len = context->max_len;
        ret = 1;
        LOGE("extend to max len, len: %d, extend_len: %d \n",
             context->len, extend_len);
    }

    ptr = realloc(context->buf, extend_len);
    if (!ptr) {
        LOGE("realloc failed \n");
        return -1;
    }

    context->buf = ptr;
    context->len = extend_len;

    return ret;
}

hy_s32_t HyDynamicArrayWriteVprintf(void *handle, const char *format, va_list *args)
{
    assert(handle);
    assert(format);
    assert(args);
#define _get_dynamic_info()                                 \
do {                                                    \
    va_copy(ap, *args);                                 \
    free_len = context->len - context->cur_len - 1;     \
    ptr = context->buf + context->write_pos;            \
} while (0)

    va_list ap;
    hy_s32_t free_len;
    hy_s32_t ret;
    char *ptr = NULL;
    dynamic_array_s *context = handle;

    _get_dynamic_info();
    ret = vsnprintf(ptr, free_len, format, ap);
    if (ret < 0) {
        LOGE("vsnprintf failed \n");
        ret = -1;
    } else if (ret >= 0) {
        if (ret < free_len) {
            context->cur_len += ret;
            context->write_pos += ret;
        } else {
            ret = _dynamic_array_extend(context,
                                        ret - (context->len - context->cur_len));
            if (-1 == ret) {
                LOGI("_dynamic_array_extend failed \n");
            } else if (ret >= 0) {
                _get_dynamic_info();
                ret = vsnprintf(ptr, free_len, format, ap);
                context->cur_len += ret;
                context->write_pos += ret;

                /* @fixme: <22-04-23, uos> 做进一步判断 */
            }
        }
    }

    return ret;
}

hy_s32_t HyDynamicArrayWrite(void *handle, const void *buf, hy_u32_t len)
{
#define _write_data_com(_buf, _len)                 \
do {                                            \
    char *ptr = NULL;                           \
    ptr = context->buf + context->write_pos;    \
    memcpy(ptr, _buf, _len);                    \
    context->cur_len      += _len;              \
    context->write_pos    += _len;              \
} while (0)

    assert(handle);
    assert(buf);

    dynamic_array_s *context = handle;
    hy_s32_t ret = 0;

    if (context->len - context->cur_len > len) {
        _write_data_com(buf, len);
    } else {
        ret = _dynamic_array_extend(context,
                                    len - (context->len - context->cur_len));
        if (-1 == ret) {
            LOGI("_dynamic_array_extend failed \n");
            len = -1;
        } else if (0 == ret) {
            _write_data_com(buf, len);
        } else {
            // 3 for "..." 1 for "\0"
            len = context->len - context->cur_len - 3 - 1;
            _write_data_com(buf, len);

            LOGI("truncated data \n");
            _write_data_com("...", 3);
            len += 3;
        }
    }

    return len;
}

void HyDynamicArrayReset(void *handle)
{
    HY_ASSERT_RET(handle);

    dynamic_array_s *context = handle;

    HY_MEMSET(context->buf, context->len);
    context->cur_len = 0;
    context->write_pos = 0;
    context->read_pos = 0;
}

void HyDynamicArrayDestroy(void **context_pp)
{
    HY_ASSERT_RET(!context_pp);
    HY_ASSERT_RET(!*context_pp);

    dynamic_array_s *context = *context_pp;

    LOGI("dynamic array context: %p destroy, buf: %p \n",
         context, context->buf);

    HY_MEM_FREE_PP(&context->buf);
    HY_MEM_FREE_PP(context_pp);
}

void *HyDynamicArrayCreate(hy_u32_t min_len, hy_u32_t max_len)
{
    HY_ASSERT_RET_VAL(min_len == 0, NULL);
    HY_ASSERT_RET_VAL(max_len == 0, NULL);
    HY_ASSERT_RET_VAL(min_len > max_len, NULL);

    dynamic_array_s *context = NULL;
    do {
        context = HY_MEM_CALLOC_BREAK(dynamic_array_s *, sizeof(*context));
        context->buf = HY_MEM_CALLOC_BREAK(char *, min_len);

        context->max_len    = max_len;
        context->min_len    = min_len;
        context->len        = context->min_len;
        context->write_pos  = context->read_pos = 0;

        LOGI("dynamic array context: %p create, buf: %p \n",
             context, context->buf);
        return context;
    } while (0);

    LOGE("dynamic array context: %p create failed \n", context);
    HyDynamicArrayDestroy((void **)&context);
    return NULL;
}

