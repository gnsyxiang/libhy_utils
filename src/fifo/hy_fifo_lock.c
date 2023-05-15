/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_fifo_lock.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    15/05 2023 20:12
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        15/05 2023      create the file
 * 
 *     last modified: 15/05 2023 20:12
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_barrier.h"
#include "hy_mem.h"
#include "hy_hex.h"
#include "hy_utils.h"
#include "thread/hy_thread_mutex.h"
#include "thread/hy_thread_cond.h"

#include "hy_fifo_lock.h"

/**
 * @brief 内存屏障
 *
 * 解决两个问题:
 * 1, 解决内存可见性问题
 * 2, 解决cpu重排的问题，cpu优化
 */
#define USE_MB

#define _FIFO_USED_LEN(_handle)     ((_handle)->in - (_handle)->out)
#define _FIFO_FREE_LEN(_handle)     ((_handle)->save_c.len - (_FIFO_USED_LEN(_handle)))
#define _FIFO_IN_POS(_handle)       ((_handle)->in & ((_handle)->save_c.len - 1))   // 优化 _handle->write_pos % _handle->save_c.len
#define _FIFO_OUT_POS(_handle)      ((_handle)->out & ((_handle)->save_c.len - 1))    // 优化 _handle->read_pos % _handle->save_c.len

struct HyFifoLock_s {
    HyFifoLockSaveConfig_s     save_c;

    char                        *buf;       ///< 
    hy_u32_t                    out;        ///< 队头
    hy_u32_t                    in;         ///< 队尾
    HyThreadMutex_s             *mutex_h;
    HyThreadCond_s              *cond_h;
};

hy_s32_t HyFifoLockWrite(HyFifoLock_s *handle, const void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);
    hy_u32_t l;

    HyThreadMutexLock_m(handle->mutex_h);
    while (len > _FIFO_FREE_LEN(handle)) {
        HyThreadCondWait_m(handle->cond_h, handle->mutex_h, 0);
    }

    l = HY_UTILS_MIN(len, handle->save_c.len - _FIFO_IN_POS(handle));
    memcpy(handle->buf + _FIFO_IN_POS(handle), buf, l);
    memcpy(handle->buf, buf + l, len - l);

    handle->in += len;
    HyThreadMutexUnLock_m(handle->mutex_h);

    return len;
}

hy_s32_t HyFifoLockReadPeek(HyFifoLock_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);
    hy_u32_t l;

    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(handle));

    l = HY_UTILS_MIN(len, handle->save_c.len - _FIFO_OUT_POS(handle));
    memcpy(buf, handle->buf + _FIFO_OUT_POS(handle), l);
    memcpy(buf + l, handle->buf, len - l);

    return len;
}

hy_s32_t HyFifoLockRead(HyFifoLock_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);
    hy_u32_t l;

    HyThreadMutexLock_m(handle->mutex_h);
    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(handle));

    l = HY_UTILS_MIN(len, handle->save_c.len - _FIFO_OUT_POS(handle));
    memcpy(buf, handle->buf + _FIFO_OUT_POS(handle), l);
    memcpy(buf + l, handle->buf, len - l);
    handle->out += len;
    HyThreadMutexUnLock_m(handle->mutex_h);

    HyThreadCondSignal_m(handle->cond_h);

    return len;
}

hy_s32_t HyFifoLockReadDel(HyFifoLock_s *handle, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(handle));
    handle->out += len;

    return len;
}

void HyFifoLockReset(HyFifoLock_s *handle)
{
    HY_ASSERT_RET(!handle);

    handle->in = 0;
    handle->out = 0;
    HY_MEMSET(handle->buf, handle->save_c.len);
}

void HyFifoLockDumpAll(HyFifoLock_s *handle)
{
    HY_ASSERT_RET(!handle);

    HY_HEX_ASCII(handle->buf, handle->save_c.len);
}

void HyFifoLockDumpContent(HyFifoLock_s *handle)
{
    HY_ASSERT_RET(!handle);

    hy_u32_t len;

    len = handle->save_c.len - _FIFO_OUT_POS(handle);
    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(handle));

    LOGD("used len: %u, write_pos: %u, read_pos: %u \n",
         _FIFO_USED_LEN(handle), handle->in, handle->out);

    HyHex(handle->buf + _FIFO_OUT_POS(handle), len, 1);
    HyHex(handle->buf, _FIFO_USED_LEN(handle) - len, 1);
}

hy_s32_t HyFifoLockGetFreeLen(HyFifoLock_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return _FIFO_FREE_LEN(handle);
}

hy_s32_t HyFifoLockGetTotalLen(HyFifoLock_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->save_c.len;
}

hy_s32_t HyFifoLockGetUsedLen(HyFifoLock_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return _FIFO_USED_LEN(handle);
}

hy_s32_t HyFifoLockIsEmpty(HyFifoLock_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->in == handle->out;
}

hy_s32_t HyFifoLockIsFull(HyFifoLock_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->save_c.len == _FIFO_USED_LEN(handle);
}

void HyFifoLockDestroy(HyFifoLock_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyFifoLock_s *handle = *handle_pp;

    HyThreadCondBroadcast_m(handle->cond_h);

    HyThreadCondDestroy(&handle->cond_h);
    HyThreadMutexDestroy(&handle->mutex_h);

    HY_MEM_FREE_PP(&handle->buf);

    LOGI("fifo destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyFifoLock_s *HyFifoLockCreate(HyFifoLockConfig_s *fifo_c)
{
    HY_ASSERT_RET_VAL(!fifo_c, NULL);
    HyFifoLock_s *handle = NULL;

    do {
        if (!HY_UTILS_IS_POWER_OF_2(fifo_c->save_c.len)) {
            LOGW("old fifo len: %d \n", fifo_c->save_c.len);
            fifo_c->save_c.len = HyUtilsNumTo2N(fifo_c->save_c.len);
            LOGW("len must be power of 2, new fifo len: %d \n", fifo_c->save_c.len);
        }

        handle = HY_MEM_MALLOC_BREAK(HyFifoLock_s *, sizeof(*handle));
        handle->buf = HY_MEM_MALLOC_BREAK(char *, fifo_c->save_c.len);
        HY_MEMCPY(&handle->save_c, &fifo_c->save_c, sizeof(fifo_c->save_c));

        handle->in = 0;
        handle->out = 0;

        handle->mutex_h = HyThreadMutexCreate_m();
        if (!handle->mutex_h) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        handle->cond_h = HyThreadCondCreate_m();
        if (!handle->cond_h) {
            LOGE("HyThreadCondCreate_m failed \n");
            break;
        }

        LOGI("fifo create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("fifo create failed \n");
    HyFifoLockDestroy(&handle);
    return NULL;
}
