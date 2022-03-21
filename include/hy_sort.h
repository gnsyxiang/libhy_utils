/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_sort.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 13:46
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 13:46
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_SORT_H_
#define __LIBHY_UTILS_INCLUDE_HY_SORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

/**
 * @brief 排序的回调函数
 *
 * @param src: 排序数src
 * @param dst: 排序数dst
 */
typedef hy_s32_t (*HySortSwapCb_t)(void *src, void *dst);

/**
 * @brief 冒泡排序
 *
 * @param array: 需要排序的结构
 * @param len: 长度
 * @param item_len: 单个数据结构的长度
 * @param swap_cb: 用户提供的比较函数
 */
void HySortBubble(void *array, size_t len, size_t item_len,
        HySortSwapCb_t swap_cb);

/**
 * @brief 快速排序
 *
 * @param array: 需要排序的结构
 * @param low: 待排结构的索引最小值
 * @param high: 待排结构的索引最大值
 * @param item_len: 单个数据结构的长度
 * @param swap_cb: 用户提供的比较函数
 */
void HySortQuick(void *array, hy_s32_t low, hy_s32_t high,
        size_t item_len, HySortSwapCb_t swap_cb);

#ifdef __cplusplus
}
#endif

#endif

