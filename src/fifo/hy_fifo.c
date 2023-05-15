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

#define _FIFO_USED_LEN(_handle)     ((_handle)->in - (_handle)->out)
#define _FIFO_FREE_LEN(_handle)     ((_handle)->save_c.len - (_FIFO_USED_LEN(_handle)))
#define _FIFO_IN_POS(_handle)       ((_handle)->in & ((_handle)->save_c.len - 1))   // 优化 _handle->write_pos % _handle->save_c.len
#define _FIFO_OUT_POS(_handle)      ((_handle)->out & ((_handle)->save_c.len - 1))    // 优化 _handle->read_pos % _handle->save_c.len

struct HyFifo_s {
    HyFifoSaveConfig_s  save_c;

    char                *buf;       ///< 
    hy_u32_t            out;        ///< 队头
    hy_u32_t            in;         ///< 队尾
};

hy_s32_t HyFifoWrite(HyFifo_s *handle, const void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);
    hy_u32_t l;

    if (len > _FIFO_FREE_LEN(handle)) {
        LOGW("not enough space to write \n");

        len = _FIFO_FREE_LEN(handle);
    }

    // len = HY_UTILS_MIN(len, _FIFO_FREE_LEN(handle));

    // 在开始将字节放入fifo之前，请确保对out索引进行采样。
    __sync_synchronize();

    l = HY_UTILS_MIN(len, handle->save_c.len - _FIFO_IN_POS(handle));
    memcpy(handle->buf + _FIFO_IN_POS(handle), buf, l);
    memcpy(handle->buf, buf + l, len - l);

    // 在更新索引中的fifo->in之前，请确保将字节添加到fifo中。
    __sync_synchronize();

    handle->in += len;

    return len;
}

hy_s32_t HyFifoReadPeek(HyFifo_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);
    hy_u32_t l;

    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(handle));

    // 在开始从fifo中删除字节之前，请确保对索引中的in进行采样。
    __sync_synchronize();

    l = HY_UTILS_MIN(len, handle->save_c.len - _FIFO_OUT_POS(handle));
    memcpy(buf, handle->buf + _FIFO_OUT_POS(handle), l);
    memcpy(buf + l, handle->buf, len - l);

    // 在更新fifo->out索引之前，请确保从kfifo中删除字节。
    __sync_synchronize();

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

    len = HY_UTILS_MIN(len, _FIFO_USED_LEN(handle));
    handle->out += len;

    return len;
}

void HyFifoReset(HyFifo_s *handle)
{
    HY_ASSERT_RET(!handle);

    handle->in = 0;
    handle->out = 0;
    HY_MEMSET(handle->buf, handle->save_c.len);
}

void HyFifoDumpAll(HyFifo_s *handle)
{
    HY_ASSERT_RET(!handle);

    HY_HEX_ASCII(handle->buf, handle->save_c.len);
}

void HyFifoDumpContent(HyFifo_s *handle)
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

hy_s32_t HyFifoGetFreeLen(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return _FIFO_FREE_LEN(handle);
}

hy_s32_t HyFifoGetTotalLen(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->save_c.len;
}

hy_s32_t HyFifoGetUsedLen(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return _FIFO_USED_LEN(handle);
}

hy_s32_t HyFifoIsEmpty(HyFifo_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->in == handle->out;
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

        handle->in = 0;
        handle->out = 0;

        LOGI("fifo create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("fifo create failed \n");
    HyFifoDestroy(&handle);
    return NULL;
}
