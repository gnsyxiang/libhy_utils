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

#include <hy_log/hy_log.h>

#include "hy_assert.h"
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
// #define USE_MB

#define _FIFO_USED_LEN(_handle)     ((_handle)->in - (_handle)->out)
#define _FIFO_FREE_LEN(_handle)     ((_handle)->save_c.capacity - (_FIFO_USED_LEN(_handle)))
#define _FIFO_IN_POS(_handle)       ((_handle)->in & ((_handle)->save_c.capacity - 1))   // 优化 _handle->write_pos % _handle->save_c.len
#define _FIFO_OUT_POS(_handle)      ((_handle)->out & ((_handle)->save_c.capacity - 1))    // 优化 _handle->read_pos % _handle->save_c.len

struct HyFifoLock_s {
    HyFifoLockSaveConfig_s      save_c;

    char                        *buf;
    hy_u32_t                    out;
    hy_u32_t                    in;

    hy_s32_t                    is_exit;
    HyThreadMutex_s             *full_mutex_h;
    HyThreadCond_s              *full_cond_h;
    HyThreadMutex_s             *empty_mutex_h;
    HyThreadCond_s              *empty_cond_h;
};

hy_s32_t HyFifoLockWrite(HyFifoLock_s *handle, const void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);
    hy_u32_t l;

    if (handle->is_exit) {
        LOGE("the lock fifo is exit \n");
        return 0;
    }

    HyThreadMutexLock_m(handle->full_mutex_h);

    while (len > _FIFO_FREE_LEN(handle)) {
        HyThreadCondWait_m(handle->full_cond_h, handle->full_mutex_h, 0);

        if (handle->is_exit) {
            HyThreadMutexUnLock_m(handle->empty_mutex_h);

            LOGE("the lock fifo is exit \n");
            return 0;
        }
    }

    l = HY_UTILS_MIN(len, handle->save_c.capacity - _FIFO_IN_POS(handle));
    memcpy(handle->buf + _FIFO_IN_POS(handle), buf, l);
    memcpy(handle->buf, buf + l, len - l);

    handle->in += len;

    HyThreadMutexUnLock_m(handle->full_mutex_h);

    HyThreadCondSignal_m(handle->empty_cond_h);

    return len;
}

hy_s32_t HyFifoLockReadPeek(HyFifoLock_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);
    hy_u32_t l;

    if (handle->is_exit) {
        LOGE("the lock fifo is exit \n");
        return 0;
    }

    HyThreadMutexLock_m(handle->empty_mutex_h);

    while (len > _FIFO_USED_LEN(handle)) {
        HyThreadCondWait_m(handle->empty_cond_h, handle->empty_mutex_h, 0);

        if (handle->is_exit) {
            HyThreadMutexUnLock_m(handle->empty_mutex_h);

            LOGE("the lock fifo is exit \n");
            return 0;
        }
    }

    l = HY_UTILS_MIN(len, handle->save_c.capacity - _FIFO_OUT_POS(handle));
    memcpy(buf, handle->buf + _FIFO_OUT_POS(handle), l);
    memcpy(buf + l, handle->buf, len - l);

    HyThreadMutexUnLock_m(handle->empty_mutex_h);

    return len;
}

hy_s32_t HyFifoLockRead(HyFifoLock_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);
    hy_u32_t l;

    if (handle->is_exit) {
        LOGE("the lock fifo is exit \n");
        return 0;
    }

    HyThreadMutexLock_m(handle->empty_mutex_h);

    while (len > _FIFO_USED_LEN(handle)) {
        HyThreadCondWait_m(handle->empty_cond_h, handle->empty_mutex_h, 0);

        if (handle->is_exit) {
            HyThreadMutexUnLock_m(handle->empty_mutex_h);

            LOGE("the lock fifo is exit \n");
            return 0;
        }
    }

    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(handle));

    l = HY_UTILS_MIN(len, handle->save_c.capacity - _FIFO_OUT_POS(handle));
    memcpy(buf, handle->buf + _FIFO_OUT_POS(handle), l);
    memcpy(buf + l, handle->buf, len - l);
    handle->out += len;

    HyThreadMutexUnLock_m(handle->empty_mutex_h);

    HyThreadCondSignal_m(handle->full_cond_h);

    return len;
}

hy_s32_t HyFifoLockReadDel(HyFifoLock_s *handle, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    if (handle->is_exit) {
        LOGE("the lock fifo is exit \n");
        return 0;
    }

    HyThreadMutexLock_m(handle->empty_mutex_h);
    while (len > _FIFO_USED_LEN(handle)) {
        HyThreadCondWait_m(handle->empty_cond_h, handle->empty_mutex_h, 0);

        if (handle->is_exit) {
            HyThreadMutexUnLock_m(handle->empty_mutex_h);

            LOGE("the lock fifo is exit \n");
            return 0;
        }
    }

    handle->out += len;

    HyThreadMutexUnLock_m(handle->empty_mutex_h);

    HyThreadCondSignal_m(handle->full_cond_h);

    return len;
}

void HyFifoLockReset(HyFifoLock_s *handle)
{
    HY_ASSERT_RET(!handle);

    if (handle->is_exit) {
        LOGE("the lock fifo is exit \n");
        return ;
    }

    HyThreadMutexLock_m(handle->empty_mutex_h);
    HyThreadMutexLock_m(handle->full_mutex_h);
    handle->in = 0;
    handle->out = 0;
    HY_MEMSET(handle->buf, handle->save_c.capacity);
    HyThreadMutexUnLock_m(handle->full_mutex_h);
    HyThreadMutexUnLock_m(handle->empty_mutex_h);
}

void HyFifoLockDumpAll(HyFifoLock_s *handle)
{
    HY_ASSERT_RET(!handle);

    HY_HEX_ASCII(handle->buf, handle->save_c.capacity);
}

void HyFifoLockDumpContent(HyFifoLock_s *handle)
{
    HY_ASSERT_RET(!handle);

    hy_u32_t len;
    hy_u32_t out = handle->out & (handle->save_c.capacity - 1);
    hy_u32_t used_len = handle->in - handle->out;
    hy_u32_t buf_len = 10 * handle->save_c.capacity;
    hy_u32_t ret = 0;
    hy_s32_t cnt = 0;
    const hy_u8_t *str;

    char *buf = HY_MEM_CALLOC_RETURN(char *, buf_len);

    len = handle->save_c.capacity - out;
    len = HY_UTILS_MIN(len, used_len);

    LOGD("used len: %u, write_pos: %u, read_pos: %u \n",
         used_len, handle->in, handle->out);

    str = (const hy_u8_t *)(handle->buf + out);
    for (hy_u32_t i = 0; i < len; i++) {
        if (str[i] == 0x0d || str[i] == 0x0a || str[i] < 32 || str[i] >= 127) {
            ret += snprintf(buf + ret, buf_len - ret, "%02x[ ]  ", str[i]);
        } else {
            ret += snprintf(buf + ret, buf_len - ret, "%02x[%c]  ", str[i], str[i]);
        }

        cnt++;
        if (cnt == 16) {
            cnt = 0;
            ret += snprintf(buf + ret, buf_len - ret, "\r\n");
        }
    }

    str = (const hy_u8_t *)handle->buf;
    for (hy_u32_t i = 0; i < used_len - len; i++) {
        if (str[i] == 0x0d || str[i] == 0x0a || str[i] < 32 || str[i] >= 127) {
            ret += snprintf(buf + ret, buf_len - ret, "%02x[ ]  ", str[i]);
        } else {
            ret += snprintf(buf + ret, buf_len - ret, "%02x[%c]  ", str[i], str[i]);
        }

        cnt++;
        if (cnt == 16) {
            cnt = 0;
            ret += snprintf(buf + ret, buf_len - ret, "\r\n");
        }
    }
    ret += snprintf(buf + ret, buf_len - ret, "\r\n");

    LOGI("len: %d \n%s\n", used_len, buf);

    HY_MEM_FREE_PP(&buf);
}

hy_s32_t HyFifoLockGetTotalLen(HyFifoLock_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->save_c.capacity;
}

hy_s32_t HyFifoLockGetUsedLen(HyFifoLock_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return _FIFO_USED_LEN(handle);
}

hy_s32_t HyFifoLockGetFreeLen(HyFifoLock_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return _FIFO_FREE_LEN(handle);
}

hy_s32_t HyFifoLockIsEmpty(HyFifoLock_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->in == handle->out;
}

hy_s32_t HyFifoLockIsFull(HyFifoLock_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->save_c.capacity == _FIFO_USED_LEN(handle);
}

void HyFifoLockDestroy(HyFifoLock_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyFifoLock_s *handle = *handle_pp;

    handle->is_exit = 1;

    HyThreadCondBroadcast_m(handle->empty_cond_h);
    HyThreadCondBroadcast_m(handle->full_cond_h);

    HyThreadCondDestroy(&handle->empty_cond_h);
    HyThreadCondDestroy(&handle->full_cond_h);

    HyThreadMutexDestroy(&handle->full_mutex_h);
    HyThreadMutexDestroy(&handle->empty_mutex_h);

    HY_MEM_FREE_PP(&handle->buf);

    LOGI("lock fifo destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyFifoLock_s *HyFifoLockCreate(HyFifoLockConfig_s *fifo_lock_c)
{
    HY_ASSERT_RET_VAL(!fifo_lock_c, NULL);
    HyFifoLock_s *handle = NULL;

    do {
        HyFifoLockSaveConfig_s *save_c = &fifo_lock_c->save_c;
        if (!HY_UTILS_IS_POWER_OF_2(save_c->capacity)) {
            LOGW("old fifo len: %d \n", save_c->capacity);

            save_c->capacity = HyUtilsNumTo2N(save_c->capacity);
            LOGW("len must be power of 2, new fifo len: %d \n", save_c->capacity);
        }

        handle = HY_MEM_MALLOC_BREAK(HyFifoLock_s *, sizeof(*handle));
        handle->buf = HY_MEM_MALLOC_BREAK(char *, save_c->capacity);
        HY_MEMCPY(&handle->save_c, save_c, sizeof(*save_c));

        handle->in = 0;
        handle->out = 0;

        handle->empty_mutex_h = HyThreadMutexCreate_m();
        if (!handle->empty_mutex_h) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        handle->empty_cond_h = HyThreadCondCreate_m();
        if (!handle->empty_cond_h) {
            LOGE("HyThreadCondCreate_m failed \n");
            break;
        }

        handle->full_mutex_h = HyThreadMutexCreate_m();
        if (!handle->full_mutex_h) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        handle->full_cond_h = HyThreadCondCreate_m();
        if (!handle->full_cond_h) {
            LOGE("HyThreadCondCreate_m failed \n");
            break;
        }

        LOGI("lock fifo create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("lock fifo create failed \n");
    HyFifoLockDestroy(&handle);
    return NULL;
}
