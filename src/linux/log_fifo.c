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
#include <pthread.h>

#include "log_fifo.h"

#include "hy_utils.h"

#include "hy_hal/hy_barrier.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

/**
 * @brief 内存屏障
 *
 * 解决两个问题:
 * 1, 解决内存可见性问题
 * 2, 解决cpu重排的问题，cpu优化
 */
#define USE_MB

#define _FIFO_USED_LEN(context)     (context->write_pos - context->read_pos)
#define _FIFO_FREE_LEN(context)     (context->len - (_FIFO_USED_LEN(context)))
#define _FIFO_WRITE_POS(context)    (context->write_pos & (context->len - 1))   // 优化 context->write_pos % context->len
#define _FIFO_READ_POS(context)     (context->read_pos & (context->len - 1))    // 优化 context->read_pos % context->len
#define _FIFO_IS_EMPTY(context)     (context->read_pos == context->write_pos)

typedef struct {
    hy_u32_t            len;
    char                *buf;

    hy_u32_t            read_pos;
    hy_u32_t            write_pos;

    pthread_mutex_t     mutex;
} _fifo_context_t;

hy_u32_t log_fifo_write(void *handle, void *buf, hy_u32_t len)
{
    if (!handle || !buf || len == 0) {
        return 0;
    }
    _fifo_context_t *context = handle;
    hy_u32_t len_tmp = 0;

    if (len > _FIFO_FREE_LEN(context)) {
        printf("write failed, len: %u, free_len: %u \n",
                len, _FIFO_FREE_LEN(context));
        return 0;
    }

#ifdef USE_MB
    // 确保其他线程对write_pos的可见性
    HY_SMP_MB();
#endif

    pthread_mutex_lock(&context->mutex);

    len_tmp = HY_UTILS_MIN(len, context->len - _FIFO_WRITE_POS(context));

    memcpy(context->buf + _FIFO_WRITE_POS(context), buf, len_tmp);
    memcpy(context->buf, buf + len_tmp, len - len_tmp);

#ifdef USE_MB
    // 确保write_pos不会优化到上面去
    HY_SMP_WMB();
#endif

    context->write_pos += len;

    pthread_mutex_unlock(&context->mutex);

    return len;
}

hy_u32_t log_fifo_read(void *handle, void *buf, hy_u32_t len)
{
    if (!handle || !buf || len == 0) {
        return 0;
    }
    _fifo_context_t *context = handle;

    hy_u32_t len_tmp = 0;

    if (_FIFO_IS_EMPTY(context)) {
        printf("read failed, fifo is empty \n");
        return 0;
    }

    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(context));

#ifdef USE_MB
    // 确保其他线程对read_pos的可见性
    HY_SMP_WMB();
#endif

    len_tmp = HY_UTILS_MIN(len, context->len - _FIFO_READ_POS(context));

    memcpy(buf, context->buf + _FIFO_READ_POS(context), len_tmp);
    memcpy(buf + len_tmp, context->buf, len - len_tmp);

#ifdef USE_MB
    // 确保read_pos不会优化到上面去
    HY_SMP_MB();
#endif

    pthread_mutex_lock(&context->mutex);
    context->read_pos += len;
    pthread_mutex_unlock(&context->mutex);

    return len;
}

hy_s32_t log_fifo_is_empty(void *handle)
{
    if (!handle) {
        return 0;
    }
    _fifo_context_t *context = handle;

    return _FIFO_IS_EMPTY(context);
}

void log_fifo_destroy(void **handle)
{
    if (!handle || !*handle) {
        return;
    }
    _fifo_context_t *context = *handle;

    pthread_mutex_destroy(&context->mutex);

    HY_MEM_FREE_P(context->buf);
    HY_MEM_FREE_PP(handle);
}

void *log_fifo_create(hy_u32_t len)
{
    _fifo_context_t *context = NULL;
    do {
        context = malloc(sizeof(*context));
        if (!context) {
            printf("malloc failed \n");
            break;
        }
        HY_MEMSET(context, sizeof(*context));

        if (!HY_UTILS_IS_POWER_OF_2(len)) {
            len = HyUtilsNumTo2N(len);

            printf("size must be power of 2, new size: %d \n", len);
        }
        context->buf = malloc(len);
        if (!context->buf) {
            printf("malloc failed \n");
            break;
        }
        HY_MEMSET(context->buf, len);

        context->len = len;
        context->write_pos = context->read_pos = 0;

        pthread_mutex_init(&context->mutex, NULL);

        return context;
    } while (0);

    log_fifo_destroy((void **)&context);
    return NULL;
}
