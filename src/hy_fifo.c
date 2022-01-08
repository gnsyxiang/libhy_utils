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

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_barrier.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_log.h"

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

#define _FIFO_USED_LEN(context)     (context->write_pos - context->read_pos)
#define _FIFO_FREE_LEN(context)     (context->save_config.len - (_FIFO_USED_LEN(context)))
#define _FIFO_WRITE_POS(context)    (context->write_pos & (context->save_config.len - 1))   // 优化 context->write_pos % context->save_config.len
#define _FIFO_READ_POS(context)     (context->read_pos & (context->save_config.len - 1))    // 优化 context->read_pos % context->save_config.len
#define _FIFO_IS_EMPTY(context)     (context->read_pos == context->write_pos)

typedef struct {
    HyFifoSaveConfig_t  save_config;
    char                *buf;

    hy_u32_t            read_pos;
    hy_u32_t            write_pos;

    pthread_mutex_t     mutex;
} _fifo_context_t;

static const char *_dump_type[HY_FIFO_DUMP_MAX] = {
    [HY_FIFO_DUMP_ALL]          = "HY_FIFO_DUMP_ALL",
    [HY_FIFO_DUMP_CONTENT]      = "HY_FIFO_DUMP_CONTENT",
};

static const char *_info_type[HY_FIFO_INFO_MAX] = {
    [HY_FIFO_INFO_TOTAL_LEN]    = "HY_FIFO_INFO_TOTAL_LEN",
    [HY_FIFO_INFO_USED_LEN]     = "HY_FIFO_INFO_USED_LEN",
    [HY_FIFO_INFO_FREE_LEN]     = "HY_FIFO_INFO_FREE_LEN",
};

hy_u32_t HyFifoWrite(void *handle, void *buf, hy_u32_t len)
{
    LOGT("handle: %p, buf: %p, len: %d \n", handle, buf, len);
    HY_ASSERT_VAL_RET_VAL(!handle || !buf || len == 0, 0);

    _fifo_context_t *context = handle;
    hy_u32_t len_tmp = 0;

    if (len > _FIFO_FREE_LEN(context)) {
        LOGE("write failed, len: %u, free_len: %u \n",
                len, _FIFO_FREE_LEN(context));
        return 0;
    }

#ifdef USE_MB
    // 确保其他线程对write_pos的可见性
    HY_SMP_MB();
#endif

    len_tmp = HY_UTILS_MIN(len, context->save_config.len - _FIFO_WRITE_POS(context));

    if (context->save_config.mutex_flag) {
        pthread_mutex_lock(&context->mutex);
    }

    memcpy(context->buf + _FIFO_WRITE_POS(context), buf, len_tmp);
    memcpy(context->buf, buf + len_tmp, len - len_tmp);

#ifdef USE_MB
    // 确保write_pos不会优化到上面去
    HY_SMP_WMB();
#endif

    context->write_pos += len;

    if (context->save_config.mutex_flag) {
        pthread_mutex_unlock(&context->mutex);
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

    len_tmp = HY_UTILS_MIN(len, context->save_config.len - _FIFO_READ_POS(context));

    memcpy(buf, context->buf + _FIFO_READ_POS(context), len_tmp);
    memcpy(buf + len_tmp, context->buf, len - len_tmp);

#ifdef USE_MB
    // 确保read_pos不会优化到上面去
    HY_SMP_MB();
#endif

    return len;
}

hy_u32_t HyFifoRead(void *handle, void *buf, hy_u32_t len)
{
    LOGT("handle: %p, buf: %p, len: %d \n", handle, buf, len);
    HY_ASSERT_VAL_RET_VAL(!handle || !buf || len == 0, 0);
    _fifo_context_t *context = handle;

    len = _fifo_read_com(handle, buf, len);

    if (context->save_config.mutex_flag) {
        pthread_mutex_lock(&context->mutex);
    }

    context->read_pos += len;

    if (context->save_config.mutex_flag) {
        pthread_mutex_unlock(&context->mutex);
    }

    return len;
}

hy_u32_t HyFifoReadPeek(void *handle, void *buf, hy_u32_t len)
{
    LOGT("handle: %p, buf: %p, len: %d \n", handle, buf, len);
    HY_ASSERT_VAL_RET_VAL(!handle || !buf || len == 0, 0);

    return _fifo_read_com(handle, buf, len);
}

void HyFifoClean(void *handle)
{
    LOGT("handle: %p \n", handle);
    HY_ASSERT_VAL_RET(!handle);

    _fifo_context_t *context = handle;

    if (context->save_config.mutex_flag) {
        pthread_mutex_lock(&context->mutex);
    }

    context->write_pos = context->read_pos = 0;
    HY_MEMSET(context->buf, context->save_config.len);

    if (context->save_config.mutex_flag) {
        pthread_mutex_unlock(&context->mutex);
    }
}

hy_u32_t HyFifoUpdateOut(void *handle, hy_u32_t len)
{
    LOGT("handle: %p, len: %d \n", handle, len);
    HY_ASSERT_VAL_RET_VAL(!handle, 0);

    _fifo_context_t *context = handle;

    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(context));

    if (context->save_config.mutex_flag) {
        pthread_mutex_lock(&context->mutex);
    }

    context->read_pos += len;

    if (context->save_config.mutex_flag) {
        pthread_mutex_unlock(&context->mutex);
    }

    return len;
}

static void _dump_hex_ascii(char *buf, hy_s32_t len)
{
    if (len <= 0) {
        return;
    }

    for (hy_s32_t i = 0; i < len; i++) {
        if (buf[i] == 0x0d || buf[i] == 0x0a || buf[i] < 32 || buf[i] >= 127) {
            printf("%02x[ ]  ", (hy_u8_t)buf[i]);
        } else {
            printf("%02x[%c]  ", (hy_u8_t)buf[i], (hy_u8_t)buf[i]);
        }
    }
}

static void _dump_all(_fifo_context_t *context)
{
    LOGD("len: %d \n", context->save_config.len);
    _dump_hex_ascii(context->buf, context->save_config.len);
    printf("\n");
}

static void _dump_content(_fifo_context_t *context)
{
    LOGD("used len: %u, write_pos: %u, read_pos: %u \n",
            _FIFO_USED_LEN(context), context->write_pos, context->read_pos);

    hy_u32_t len_tmp;
    len_tmp = context->save_config.len - _FIFO_READ_POS(context);
    len_tmp = HY_UTILS_MIN(len_tmp, _FIFO_USED_LEN(context));

    _dump_hex_ascii(context->buf + _FIFO_READ_POS(context), len_tmp);
    _dump_hex_ascii(context->buf, _FIFO_USED_LEN(context) - len_tmp);

    printf("\n");
}

void HyFifoDump(void *handle, HyFifoDumpType_t type)
{
    LOGT("handle: %p, type: %s \n", handle, _dump_type[type]);
    HY_ASSERT_VAL_RET(!handle);

    _fifo_context_t *context = handle;
    switch (type) {
        case HY_FIFO_DUMP_ALL:
            _dump_all(context);
            break;
        case HY_FIFO_DUMP_CONTENT:
            _dump_content(context);
            break;
        default:
            LOGE("error type: %d \n", type);
    }
}

hy_u32_t HyFifoGetInfo(void *handle, HyFifoInfoType_t type)
{
    LOGT("handle: %p, type: %s \n", handle, _info_type[type]);
    HY_ASSERT_VAL_RET_VAL(!handle, 0);

    _fifo_context_t *context = handle;
    hy_u32_t val = 0;

    switch (type) {
        case HY_FIFO_INFO_TOTAL_LEN:
            val = context->save_config.len;
            break;
        case HY_FIFO_INFO_USED_LEN:
            val = _FIFO_USED_LEN(context);
            break;
        case HY_FIFO_INFO_FREE_LEN:
            val = context->save_config.len - _FIFO_USED_LEN(context);
            break;
        default:
            LOGE("the type is ERROR, type: %d \n", type);
            break;
    }

    return val;
}

hy_s32_t HyFifoIsEmpty(void *handle)
{
    LOGT("handle: %p \n", handle);
    HY_ASSERT_VAL_RET_VAL(!handle, 0);

    _fifo_context_t *context = handle;
    return _FIFO_IS_EMPTY(context);
}

hy_s32_t HyFifoIsFull(void *handle)
{
    LOGT("handle: %p \n", handle);
    HY_ASSERT_VAL_RET_VAL(!handle, 0);

    _fifo_context_t *context = handle;
    return context->save_config.len == _FIFO_USED_LEN(context);
}

void HyFifoDestroy(void **handle)
{
    LOGT("handle: %p, *handle: %p \n", handle, *handle);
    HY_ASSERT_VAL_RET(!handle || !*handle);

    _fifo_context_t *context = *handle;

    HY_MEM_FREE_P(context->buf);

    pthread_mutex_destroy(&context->mutex);

    LOGI("fifo destroy, handle: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *HyFifoCreate(HyFifoConfig_t *config)
{
    LOGT("config: %p \n", config);
    HY_ASSERT_VAL_RET_VAL(!config, NULL);

    _fifo_context_t *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_fifo_context_t *, sizeof(*context));

        if (!HY_UTILS_IS_POWER_OF_2(config->save_config.len)) {
            config->save_config.len = HyUtilsNumTo2N(config->save_config.len);

            LOGW("size must be power of 2, new size: %d \n",
                    config->save_config.len);
        }
        context->buf = HY_MEM_MALLOC_BREAK(char *, config->save_config.len);

        HY_MEMCPY(&context->save_config, &config->save_config, sizeof(config->save_config));
        context->write_pos = context->read_pos = 0;

        pthread_mutex_init(&context->mutex, NULL);

        LOGI("fifo create, handle: %p \n", context);
        return context;
    } while (0);

    HyFifoDestroy((void **)&context);
    return NULL;
}
