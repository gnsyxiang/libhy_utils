/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_fifo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 10:52
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 10:52
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hy_fifo.h"

#include "hy_utils.h"

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_log.h"

#define ALONE_DEBUG 1

typedef struct {
    char    *buf;

    size_t  size;
    size_t  use_cnt;

    size_t  in;
    size_t  out;
} _fifo_context_t;

static inline void _print_hex_ascii(char *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        if (buf[i] == 0x0d || buf[i] == 0x0a || buf[i] < 32 || buf[i] >= 127) {
            printf("%02x[ ]  ", (uint8_t)buf[i]);
        } else {
            printf("%02x[%c]  ", (uint8_t)buf[i], (uint8_t)buf[i]);
        }
    }
}

#ifdef FIFO_PRINT_ALL
static inline void _print_all_buf(_fifo_context_t *context)
{
    LOGE("len: %d \n", context->size);
    _print_hex_ascii(context->buf, context->size);
    printf("\n");
}
#endif

static inline void _print_content(_fifo_context_t *context)
{
    uint32_t fifo_len = context->in - context->out;

    LOGI("cnt: %d, in: %d, out: %d \n", fifo_len, context->in, context->out);

    #define out_index (context->out & (context->size - 1))
    uint32_t len_tmp = HyUtilsMinMacro(fifo_len, context->size- out_index);

    _print_hex_ascii(context->buf + out_index, len_tmp);
    _print_hex_ascii(context->buf, fifo_len - len_tmp);

    printf("\n");
}

static uint32_t _get_data_com(_fifo_context_t *context, const char *buf, uint32_t len)
{
#if 0
    /**
     * @brief 保存tail副本
     *
     * @note: 1, 避免下面宏两次读取tail变量，造成多线程引起的错误
     *        2，如果使用的是函数，就不需要保存tail副本，函数此时最好加上inline提高效率
     *        3，即使tail变量被其他线程更新也不会出现程序错误，最多就是少处理一点点数据，但数据还在fifo中
     */
    uint32_t head_tmp = context->head;

    // 比较获取的数据长度和实际fifo中存在的数据长度
    len = HyUtilsMinMacro(len, head_tmp - context->tail);
#endif

    len = HyUtilsMinMacro(len, context->in - context->out);

    #define out_index (context->out & (context->size - 1))
    uint32_t len_tmp = HyUtilsMinMacro(len, context->size - out_index);

    memcpy((void *)buf, context->buf + out_index, len_tmp);
    memcpy((void *)(buf + len_tmp), context->buf, len - len_tmp);

    return len;
}

//---------------------------------------------------------------------------

size_t HyFifoPut(void *handle, void *buf, size_t len)
{
    // assert(handle || buf || len);
    // if (!handle || !buf || 0 == len) {
        // LOGE("the param is NULL, handle: %p, buf: %p, len: %d \n", handle, buf, len);
        // return 0;
    // }
    _fifo_context_t *context = handle;

#if 0
    /**
     * @brief 保存tail副本
     *
     * @note: 1, 避免下面宏两次读取tail变量，造成多线程引起的错误
     *        2，如果使用的是函数，就不需要保存tail副本，函数此时最好加上inline提高效率
     *        3，即使tail变量被其他线程更新也不会出现程序错误，最多就是少处理一点点数据，但数据还在fifo中
     */
    uint32_t tail_tmp = context->tail;

    // 比较存取的数据长度和实际fifo中可被存入的数据长度
    len = HyUtilsMinMacro(len, context->len - context->head + tail_tmp);
#endif
    len = HyUtilsMinMacro(len, context->size - context->in + context->out);

    #define in_index (context->in & (context->size - 1))
    size_t len_tmp = HyUtilsMinMacro(len, context->size - in_index);

    memcpy(context->buf + in_index, buf, len_tmp);
    memcpy(context->buf, (char *)buf + len_tmp, len - len_tmp);
    context->in += len;
    context->use_cnt += len;

    return len;
}

size_t HyFifoGet(void *handle, void *buf, size_t len)
{
    if (!handle || !buf || 0 == len) {
        LOGE("the param is NULL, handle: %p, buf: %p, len: %d \n", handle, buf, len);
        return 0;
    }
    _fifo_context_t *context = handle;
    len = _get_data_com(context, buf, len);
    context->out += len;
    context->use_cnt -= len;

    return len;
}

size_t HyFifoPeek(void *handle, void *buf, size_t len)
{
    if (!handle || !buf || 0 == len) {
        LOGE("the param is NULL, handle: %p, buf: %p, len: %d \n", handle, buf, len);
        return 0;
    }
    _fifo_context_t *context = handle;
    len = _get_data_com(context, buf, len);

    return len;
}

size_t HyFifoUpdateOut(void *handle, size_t len)
{
    if (!handle || 0 == len) {
        LOGE("the param is NULL, handle: %p, len: %d \n", handle, len);
        return 0;
    }
    _fifo_context_t *context = handle;

    char buf[1024] = {0};
    len = _get_data_com(context, buf, len);
    context->out += len;

    return len;
}

void HyFifoGetInfo(void *handle, HyFifoInfoType_t type, void *val)
{
    HY_ASSERT_VAL_RET(!handle || !val);

    _fifo_context_t *context = handle;

    switch (type) {
        case HY_FIFO_INFO_TOTAL_LEN:
            *((size_t *)val) = context->size;
            break;
        case HY_FIFO_INFO_USED_LEN:
            *((size_t *)val) = context->use_cnt;
            break;
        case HY_FIFO_INFO_FREE_LEN:
            *((size_t *)val) = context->size - (context->in + context->out);
            break;
        default:
            LOGE("the type is ERROR, type: %d \n", type);
            break;
    }
}

void HyFifoDump(void *handle)
{
    if (!handle) {
        LOGE("the param is NULL, handle: %p \n", handle);
        return;
    }
    _fifo_context_t *context = handle;

#ifdef FIFO_PRINT_ALL
    _print_all_buf(context);
#endif
    _print_content(context);
}

void HyFifoClean(void *handle)
{
    HY_ASSERT_VAL_RET(!handle);

    _fifo_context_t *context = handle;

    memset(context->buf, '\0', context->size);
    context->in = context->out = 0;
}

void HyFifoDestroy(void **handle)
{
    HY_ASSERT_VAL_RET(!handle || !*handle);

    _fifo_context_t *context = *handle;

    if (context->buf) {
        HY_MEM_FREE_PP(&context->buf);
    }

    HY_MEM_FREE_PP(handle);

    LOGI("fifo destroy successful \n");
}

static uint32_t _HyUtilsNumTo2N2(uint32_t num)
{
    uint32_t i = 1;
    uint32_t num_tmp = num;

    while (num >>= 1) {
        i <<= 1;
    }

    return (i < num_tmp) ? i << 1 : i;
}

void *HyFifoCreate(HyFifoConfig_t *config)
{
    HY_ASSERT_VAL_RET_VAL(!config, NULL);

    _fifo_context_t *context = NULL;
    do {
        size_t size = config->save_config.size;

        if (!HyUtilsIsPowerOf2(size) || size > 0x80000000) {
            size = _HyUtilsNumTo2N2(size);
            LOGE("size must be power of 2, new size: %d \n", size);
        }

        context = HY_MEM_MALLOC_BREAK(_fifo_context_t *, sizeof(*context));
        context->buf = HY_MEM_MALLOC_BREAK(char *, size);

        context->size = size;

        LOGI("fifo create successful \n");
        return context;
    } while (0);

    HyFifoDestroy((void **)&context);
    return NULL;
}

