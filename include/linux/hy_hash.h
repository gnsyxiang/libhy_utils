/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_hash.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 15:25
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 15:25
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_HASH_H_
#define __LIBHY_UTILS_INCLUDE_HY_HASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

typedef void (*HyHashDumpItemCb_t)(void *val, void *args);

typedef struct {
    const char  *key;

    void        *val;
    hy_u32_t    val_len;
} HyHashItem_t;

typedef struct {
    hy_u32_t bucket_cnt;
} HyHashSaveConfig_t;

typedef struct {
    HyHashSaveConfig_t save_config;
} HyHashConfig_t;

hy_u32_t HyHashGet(const char *key);

void *HyHashCreate(HyHashConfig_t *config);
void HyHashDestroy(void **handle);

void HyHashDump(void *handle, HyHashDumpItemCb_t dump_item_cb, void *args);

hy_s32_t HyHashItemAdd(void *handle, HyHashItem_t *h_item);
hy_s32_t HyHashItemDel(void *handle, HyHashItem_t *h_item);
hy_s32_t HyHashItemGet(void *handle, HyHashItem_t *h_item);

#ifdef __cplusplus
}
#endif

#endif

