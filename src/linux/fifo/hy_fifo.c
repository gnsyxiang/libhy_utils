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

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_mem.h"
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
// #define USE_MB

struct HyFifo_s {
    HyFifoSaveConfig_s      save_c;

    char                    *buf;
    hy_u32_t                out;
    hy_u32_t                in;
};

static inline hy_u32_t _fifo_free_len(HyFifo_s *handle)
{
    return (handle->save_c.capacity - (handle->in - handle->out));
}

static void _fifo_write(HyFifo_s *handle, const void *buf, hy_u32_t len, hy_u32_t in)
{
    hy_u32_t l;

    // 优化 _handle->in % _handle->save_c.capacity，提升效率
    in &= (handle->save_c.capacity - 1);

    l = HY_UTILS_MIN(len, handle->save_c.capacity - in);

    memcpy(handle->buf + in, buf, l);
    memcpy(handle->buf, buf + l, len - l);

    /**
      * make sure that the data in the fifo is up to date before
      * incrementing the fifo->in index counter
      */
    // 保证了在SMP多处理器下，一定是先完成了fifo的内存操作，
    // 然后再进行变量的增加。以免被优化后的混乱访问，导致策略失败
    __sync_synchronize();
}

hy_s32_t HyFifoWrite(HyFifo_s *handle, const void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);
    hy_u32_t l;

    l = _fifo_free_len(handle);
    if (len > l) {
        LOGW("not enough space to write \n");

        if (l == 0) {
            LOGE("fifo is full \n");
            return 0;
        }

        len = l;
    }

    _fifo_write(handle, buf, len, handle->in);
    handle->in += len;

    return len;
}

static void _fifo_read(HyFifo_s *handle, void *buf, hy_u32_t len, hy_u32_t out)
{
    hy_u32_t l;

    // 优化 _handle->out % _handle->save_c.capacity，提升效率
    out &= (handle->save_c.capacity - 1);

    l = HY_UTILS_MIN(len, handle->save_c.capacity - out);

    memcpy(buf, handle->buf + out, l);
    memcpy(buf + l, handle->buf, len - l);

    /**
      * make sure that the data in the fifo is up to date before
      * incrementing the fifo->in index counter
      */
    // 保证了在SMP多处理器下，一定是先完成了fifo的内存操作，
    // 然后再进行变量的增加。以免被优化后的混乱访问，导致策略失败
    __sync_synchronize();
}

hy_s32_t HyFifoReadPeek(HyFifo_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);
    hy_u32_t l;

    l = handle->in - handle->out;
    if (len > l) {
        LOGW("not enough data to read \n");

        if (l == 0) {
            LOGE("fifo is empty \n");
            return 0;
        }

        len = l;
    }

    _fifo_read(handle, buf, len, handle->out);

    return len;
}

hy_s32_t HyFifoRead(HyFifo_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    len = HyFifoReadPeek(handle, buf, len);
    handle->out += len;

    return len;
}

hy_s32_t HyFifoReadDel(HyFifo_s *handle, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    len = HY_UTILS_MIN(len, handle->in - handle->out);
    handle->out += len;

    return len;
}

void HyFifoReset(HyFifo_s *handle)
{
    HY_ASSERT_RET(!handle);

    handle->in  = 0;
    handle->out = 0;
    HY_MEMSET(handle->buf, handle->save_c.capacity);
}

hy_s32_t HyFifoGetTotalLen(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->save_c.capacity;
}

hy_s32_t HyFifoGetUsedLen(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return (handle->in - handle->out);
}

hy_s32_t HyFifoGetFreeLen(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return _fifo_free_len(handle);
}

hy_s32_t HyFifoIsEmpty(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->in == handle->out;
}

hy_s32_t HyFifoIsFull(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->save_c.capacity == (handle->in - handle->out);
}

void HyFifoDumpAll(HyFifo_s *handle)
{
    HY_ASSERT_RET(!handle);

    HY_HEX_ASCII(handle->buf, handle->save_c.capacity);
}

void HyFifoDumpContent(HyFifo_s *handle)
{
    HY_ASSERT_RET(!handle);

    hy_u32_t len;
    hy_u32_t out = handle->out & (handle->save_c.capacity - 1);
    hy_u32_t free_len = handle->in - handle->out;

    len = handle->save_c.capacity - out;
    len = HY_UTILS_MIN(len, free_len);

    LOGD("used len: %u, write_pos: %u, read_pos: %u \n",
         free_len, handle->in, handle->out);

    HyHex(handle->buf + out, len, 1);
    HyHex(handle->buf, free_len - len, 1);
}

void HyFifoDestroy(HyFifo_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyFifo_s *handle = *handle_pp;

    HY_MEM_FREE_PP(&handle->buf);

    LOGI("fifo destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyFifo_s *HyFifoCreate(HyFifoConfig_s *fifo_c)
{
    HY_ASSERT_RET_VAL(!fifo_c, NULL);
    HyFifo_s *handle = NULL;

    do {
        HyFifoSaveConfig_s *save_c = &fifo_c->save_c;
        if (!HY_UTILS_IS_POWER_OF_2(save_c->capacity)) {
            LOGW("old fifo len: %d \n", save_c->capacity);

            save_c->capacity = HyUtilsNumTo2N(save_c->capacity);
            LOGW("len must be power of 2, new fifo len: %d \n", save_c->capacity);
        }

        handle = HY_MEM_MALLOC_BREAK(HyFifo_s *, sizeof(*handle));
        handle->buf = HY_MEM_MALLOC_BREAK(char *, save_c->capacity);
        HY_MEMCPY(&handle->save_c, save_c, sizeof(*save_c));

        handle->in = 0;
        handle->out = 0;

        LOGI("fifo create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("fifo create failed \n");
    HyFifoDestroy(&handle);
    return NULL;
}
