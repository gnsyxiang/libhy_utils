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

#define _FIFO_USED_LEN(handle)      ((handle)->write_pos - (handle)->read_pos)
#define _FIFO_FREE_LEN(handle)      ((handle)->save_c.len - (_FIFO_USED_LEN(handle)))
#define _FIFO_WRITE_POS(handle)     ((handle)->write_pos & ((handle)->save_c.len - 1))   // 优化 handle->write_pos % handle->save_c.len
#define _FIFO_READ_POS(handle)      ((handle)->read_pos & ((handle)->save_c.len - 1))    // 优化 handle->read_pos % handle->save_c.len
#define _FIFO_IS_EMPTY(handle)      ((handle)->read_pos == (handle)->write_pos)

struct HyFifo_s {
    HyFifoSaveConfig_s  save_c;

    char                *buf;
    hy_u32_t            read_pos;
    hy_u32_t            write_pos;
    HyThreadMutex_s     *mutex;
};

hy_s32_t HyFifoWrite(HyFifo_s *handle, const void *buf, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!handle || !buf || len == 0, -1);
    hy_u32_t len_tmp = 0;

    if (len > _FIFO_FREE_LEN(handle)) {
        LOGE("write failed, len: %u, free_len: %u \n", len, _FIFO_FREE_LEN(handle));
        return -1;
    }

#ifdef USE_MB
    // 确保其他线程对write_pos的可见性
    HY_SMP_MB();
#endif

    len_tmp = HY_UTILS_MIN(len, handle->save_c.len - _FIFO_WRITE_POS(handle));

    if (handle->save_c.is_lock) {
        HyThreadMutexLock_m(handle->mutex);
    }

    memcpy(handle->buf + _FIFO_WRITE_POS(handle), buf, len_tmp);
    memcpy(handle->buf, buf + len_tmp, len - len_tmp);

#ifdef USE_MB
    // 确保write_pos不会优化到上面去
    HY_SMP_WMB();
#endif

    handle->write_pos += len;

    if (handle->save_c.is_lock) {
        HyThreadMutexUnLock_m(handle->mutex);
    }

    return len;
}

static hy_s32_t _fifo_read_com(HyFifo_s *handle, void *buf, hy_u32_t len)
{
    hy_u32_t len_tmp = 0;

    if (_FIFO_IS_EMPTY(handle)) {
        return 0;
    }

    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(handle));

#ifdef USE_MB
    // 确保其他线程对read_pos的可见性
    HY_SMP_WMB();
#endif

    len_tmp = HY_UTILS_MIN(len, handle->save_c.len - _FIFO_READ_POS(handle));

    memcpy(buf, handle->buf + _FIFO_READ_POS(handle), len_tmp);
    memcpy(buf + len_tmp, handle->buf, len - len_tmp);

#ifdef USE_MB
    // 确保read_pos不会优化到上面去
    HY_SMP_MB();
#endif

    return len;
}

hy_s32_t HyFifoRead(HyFifo_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!handle || !buf || len == 0, -1);

    len = _fifo_read_com(handle, buf, len);

    if (handle->save_c.is_lock) {
        HyThreadMutexLock_m(handle->mutex);
    }
    handle->read_pos += len;
    if (handle->save_c.is_lock) {
        HyThreadMutexUnLock_m(handle->mutex);
    }

    return len;
}

hy_s32_t HyFifoReadPeek(HyFifo_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!handle || !buf || len == 0, -1);

    return _fifo_read_com(handle, buf, len);
}

void HyFifoReset(HyFifo_s *handle)
{
    HY_ASSERT_RET(!handle);

    if (handle->save_c.is_lock) {
        HyThreadMutexLock_m(handle->mutex);
    }
    handle->write_pos = handle->read_pos = 0;
    HY_MEMSET(handle->buf, handle->save_c.len);
    if (handle->save_c.is_lock) {
        HyThreadMutexUnLock_m(handle->mutex);
    }
}

hy_s32_t HyFifoReadDel(HyFifo_s *handle, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(handle));

    if (handle->save_c.is_lock) {
        HyThreadMutexLock_m(handle->mutex);
    }
    handle->read_pos += len;
    if (handle->save_c.is_lock) {
        HyThreadMutexUnLock_m(handle->mutex);
    }

    return len;
}

void HyFifoDumpAll(HyFifo_s *handle)
{
    HY_ASSERT_RET(!handle);

    HY_HEX_ASCII(handle->buf, handle->save_c.len);
}

void HyFifoDumpContent(HyFifo_s *handle)
{
    HY_ASSERT_RET(!handle);

    hy_u32_t len_tmp;

    len_tmp = handle->save_c.len - _FIFO_READ_POS(handle);
    len_tmp = HY_UTILS_MIN(len_tmp, _FIFO_USED_LEN(handle));

    LOGD("used len: %u, write_pos: %u, read_pos: %u \n",
         _FIFO_USED_LEN(handle), handle->write_pos, handle->read_pos);

    HyHex(handle->buf + _FIFO_READ_POS(handle), len_tmp, 1);
    HyHex(handle->buf, _FIFO_USED_LEN(handle) - len_tmp, 1);
}

hy_s32_t HyFifoGetFreeLen(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->save_c.len - _FIFO_USED_LEN(handle);
}

hy_s32_t HyFifoGetUsedLen(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return _FIFO_USED_LEN(handle);
}

hy_s32_t HyFifoIsEmpty(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return _FIFO_IS_EMPTY(handle);
}

hy_s32_t HyFifoIsFull(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->save_c.len == _FIFO_USED_LEN(handle);
}

void HyFifoDestroy(HyFifo_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyFifo_s *handle = *handle_pp;

    HY_MEM_FREE_PP(&handle->buf);

    HyThreadMutexDestroy(&handle->mutex);

    LOGI("fifo destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyFifo_s *HyFifoCreate(HyFifoConfig_s *fifo_c)
{
    HY_ASSERT_RET_VAL(!fifo_c, NULL);
    HyFifo_s *handle = NULL;

    do {
        if (!HY_UTILS_IS_POWER_OF_2(fifo_c->save_c.len)) {
            LOGW("old fifo len: %d \n", fifo_c->save_c.len);
            fifo_c->save_c.len = HyUtilsNumTo2N(fifo_c->save_c.len);
            LOGW("len must be power of 2, new fifo len: %d \n", fifo_c->save_c.len);
        }

        handle = HY_MEM_MALLOC_BREAK(HyFifo_s *, sizeof(*handle));
        handle->buf = HY_MEM_MALLOC_BREAK(char *, fifo_c->save_c.len);
        HY_MEMCPY(&handle->save_c, &fifo_c->save_c, sizeof(fifo_c->save_c));

        handle->write_pos = 0;
        handle->read_pos = 0;

        handle->mutex = HyThreadMutexCreate_m();
        if (!handle->mutex) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        LOGI("fifo create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("fifo create failed \n");
    HyFifoDestroy(&handle);
    return NULL;
}
