/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_bit.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 14:22
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 14:22
 */
#include <stdio.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"

#include "hy_bit.h"

void HyBitSet(char *byte, hy_u32_t len, hy_u32_t index)
{
    if (!byte || index / 8 > len) {
        LOGE("the param is error, byte: %p, %d / 8 > %d \n", byte, index, len);
        return;
    }

    hy_u32_t byte_index = index / 8;
    hy_u32_t bit_index = index % 8;

    byte[byte_index] |= (0x1ULL << bit_index);
}

void HyBitReSet(char *byte, hy_u32_t len, hy_u32_t index)
{
    if (!byte || index / 8 > len) {
        LOGE("the param is error, byte: %p, %d / 8 > %d \n", byte, index, len);
        return;
    }

    hy_u32_t byte_index = index / 8;
    hy_u32_t bit_index = index % 8;

    byte[byte_index] &= ~(0x1ULL << bit_index);
}

hy_s32_t HyBitGet(char *byte, hy_u32_t len, hy_u32_t index)
{
    if (!byte || index / 8 > len) {
        LOGE("the param is error, byte: %p, %d / 8 > %d \n", byte, index, len);
        return -1;
    }

    hy_u32_t byte_index = index / 8;
    hy_u32_t bit_index = index % 8;

    return (byte[byte_index] & (0x1ULL << bit_index));
}

