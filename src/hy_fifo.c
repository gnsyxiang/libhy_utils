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
#include <pthread.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_barrier.h"
#include "hy_mem.h"
#include "thread/hy_thread_mutex.h"

#include "hy_hex.h"
#include "hy_utils.h"

#include "hy_fifo.h"

/**
 * @brief 内存屏障
 *
 * 解决两个问题:
 * 1, 解决内存可见性问题
 * 2, 解决cpu重排的问题，cpu优化
 */
#define USE_MB

#define _FIFO_USED_LEN(context)     ((context)->write_pos - (context)->read_pos)
#define _FIFO_FREE_LEN(context)     ((context)->save_c.len - (_FIFO_USED_LEN(context)))
#define _FIFO_WRITE_POS(context)    ((context)->write_pos & ((context)->save_c.len - 1))   // 优化 context->write_pos % context->save_c.len
#define _FIFO_READ_POS(context)     ((context)->read_pos & ((context)->save_c.len - 1))    // 优化 context->read_pos % context->save_c.len
#define _FIFO_IS_EMPTY(context)     ((context)->read_pos == (context)->write_pos)

typedef struct {
    HyFifoSaveConfig_s  save_c;

    char                *buf;

    hy_u32_t            read_pos;
    hy_u32_t            write_pos;
    void                *mutex_h;
} _fifo_context_t;

hy_s32_t HyFifoWrite(void *handle, const void *buf, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!handle || !buf || len == 0, -1);
    _fifo_context_t *context = handle;
    hy_u32_t len_tmp = 0;

    if (len > _FIFO_FREE_LEN(context)) {
        LOGE("write failed, len: %u, free_len: %u \n",
             len, _FIFO_FREE_LEN(context));
        return -1;
    }

#ifdef USE_MB
    // 确保其他线程对write_pos的可见性
    HY_SMP_MB();
#endif

    len_tmp = HY_UTILS_MIN(len, context->save_c.len - _FIFO_WRITE_POS(context));

    if (context->save_c.is_lock) {
        HyThreadMutexLock_m(context->mutex_h);
    }

    memcpy(context->buf + _FIFO_WRITE_POS(context), buf, len_tmp);
    memcpy(context->buf, buf + len_tmp, len - len_tmp);

#ifdef USE_MB
    // 确保write_pos不会优化到上面去
    HY_SMP_WMB();
#endif

    context->write_pos += len;

    if (context->save_c.is_lock) {
        HyThreadMutexUnLock_m(context->mutex_h);
    }

    return len;
}

static hy_s32_t _fifo_read_com(void *handle, void *buf, hy_u32_t len)
{
    hy_u32_t len_tmp = 0;
    _fifo_context_t *context = handle;

    if (_FIFO_IS_EMPTY(context)) {
        return 0;
    }

    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(context));

#ifdef USE_MB
    // 确保其他线程对read_pos的可见性
    HY_SMP_WMB();
#endif

    len_tmp = HY_UTILS_MIN(len, context->save_c.len - _FIFO_READ_POS(context));

    memcpy(buf, context->buf + _FIFO_READ_POS(context), len_tmp);
    memcpy(buf + len_tmp, context->buf, len - len_tmp);

#ifdef USE_MB
    // 确保read_pos不会优化到上面去
    HY_SMP_MB();
#endif

    return len;
}

hy_s32_t HyFifoRead(void *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!handle || !buf || len == 0, -1);
    _fifo_context_t *context = handle;

    len = _fifo_read_com(handle, buf, len);

    if (context->save_c.is_lock) {
        HyThreadMutexLock_m(context->mutex_h);
    }
    context->read_pos += len;
    if (context->save_c.is_lock) {
        HyThreadMutexUnLock_m(context->mutex_h);
    }

    return len;
}

hy_s32_t HyFifoReadPeek(void *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!handle || !buf || len == 0, -1);

    return _fifo_read_com(handle, buf, len);
}

void HyFifoReset(void *handle)
{
    HY_ASSERT_RET(!handle);
    _fifo_context_t *context = handle;

    if (context->save_c.is_lock) {
        HyThreadMutexLock_m(context->mutex_h);
    }
    context->write_pos = context->read_pos = 0;
    HY_MEMSET(context->buf, context->save_c.len);
    if (context->save_c.is_lock) {
        HyThreadMutexUnLock_m(context->mutex_h);
    }
}

hy_s32_t HyFifoReadDel(void *handle, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!handle, -1);
    _fifo_context_t *context = handle;

    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(context));

    if (context->save_c.is_lock) {
        HyThreadMutexLock_m(context->mutex_h);
    }
    context->read_pos += len;
    if (context->save_c.is_lock) {
        HyThreadMutexUnLock_m(context->mutex_h);
    }

    return len;
}

static void _dump_content(_fifo_context_t *context)
{
    hy_u32_t len_tmp;

    len_tmp = context->save_c.len - _FIFO_READ_POS(context);
    len_tmp = HY_UTILS_MIN(len_tmp, _FIFO_USED_LEN(context));

    LOGD("used len: %u, write_pos: %u, read_pos: %u \n",
         _FIFO_USED_LEN(context), context->write_pos, context->read_pos);

    HyHex(context->buf + _FIFO_READ_POS(context), len_tmp, 1);
    HyHex(context->buf, _FIFO_USED_LEN(context) - len_tmp, 1);
}

void HyFifoDump(void *handle, HyFifoDump_e type)
{
    HY_ASSERT_RET(!handle);
    _fifo_context_t *context = handle;

    switch (type) {
        case HY_FIFO_DUMP_ALL:
            HY_HEX_ASCII(context->buf, context->save_c.len);
            break;
        case HY_FIFO_DUMP_CONTENT:
            _dump_content(context);
            break;
        default:
            LOGE("error type: %d \n", type);
    }
}

hy_s32_t HyFifoGetInfo(void *handle, HyFifoInfo_e type)
{
    HY_ASSERT_RET_VAL(!handle, -1);
    _fifo_context_t *context = handle;
    hy_u32_t val = 0;

    switch (type) {
        case HY_FIFO_INFO_TOTAL_LEN:
            val = context->save_c.len;
            break;
        case HY_FIFO_INFO_USED_LEN:
            val = _FIFO_USED_LEN(context);
            break;
        case HY_FIFO_INFO_FREE_LEN:
            val = context->save_c.len - _FIFO_USED_LEN(context);
            break;
        default:
            LOGE("the type is ERROR, type: %d \n", type);
            break;
    }

    return val;
}

hy_s32_t HyFifoIsEmpty(void *handle)
{
    HY_ASSERT_RET_VAL(!handle, 0);

    return _FIFO_IS_EMPTY((_fifo_context_t *)handle);
}

hy_s32_t HyFifoIsFull(void *handle)
{
    HY_ASSERT_RET_VAL(!handle, 0);

    _fifo_context_t *context = handle;
    return context->save_c.len == _FIFO_USED_LEN(context);
}

void HyFifoDestroy(void **handle)
{
    HY_ASSERT_RET(!handle || !*handle);
    _fifo_context_t *context = *handle;

    HY_MEM_FREE_P(context->buf);

    HyThreadMutexDestroy(&context->mutex_h);

    LOGI("fifo destroy, context: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *HyFifoCreate(HyFifoConfig_s *fifo_c)
{
    HY_ASSERT_RET_VAL(!fifo_c, NULL);
    _fifo_context_t *context = NULL;

    do {
        if (!HY_UTILS_IS_POWER_OF_2(fifo_c->save_c.len)) {
            LOGW("old len: %d \n", fifo_c->save_c.len);
            fifo_c->save_c.len = HyUtilsNumTo2N(fifo_c->save_c.len);
            LOGW("len must be power of 2, new len: %d \n", fifo_c->save_c.len);
        }

        context = HY_MEM_MALLOC_BREAK(_fifo_context_t *, sizeof(*context));
        context->buf = HY_MEM_MALLOC_BREAK(char *, fifo_c->save_c.len);
        HY_MEMCPY(&context->save_c, &fifo_c->save_c, sizeof(fifo_c->save_c));

        context->write_pos = context->read_pos = 0;

        context->mutex_h = HyThreadMutexCreate_m();
        if (!context->mutex_h) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        LOGI("fifo create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("fifo create failed \n");
    HyFifoDestroy((void **)&context);
    return NULL;
}

