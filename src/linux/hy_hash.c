/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_hash.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 15:35
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 15:35
 */
#include <stdio.h>
#include <pthread.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_list.h"

#include "hy_hash.h"

typedef struct {
    hy_u32_t            key_hash;
    void                *val;
    hy_u32_t            val_len;

    struct hy_hlist_node    list;
} _item_t;

typedef void (*handle_item_cb_t)(_item_t *pos, HyHashItem_t *h_item);

typedef struct {
    HyHashSaveConfig_t      save_config;

    pthread_mutex_t         *bucket_mutex;
    struct hy_hlist_head    *bucket_head;
} _hash_context_t;

hy_u32_t HyHashGet(const char *key)
{
    HY_ASSERT_RET_VAL(!key, 0);

    hy_u32_t seed = 31; // 31 131 1313 13131 131313 etc..
    hy_u32_t hash = 0;

    while (*key) {
        hash = hash * seed + (*key++);
    }

    return hash;
}

static hy_s32_t _key_to_index(_hash_context_t *context, const char *key)
{
    HY_ASSERT_RET_VAL(!key, -1);

    hy_s32_t bucket_max_len = context->save_config.bucket_cnt;
    hy_s32_t len   = HY_STRLEN(key);
    hy_s32_t index = (hy_s32_t)key[0];

    for (hy_s32_t i = 1; i < len; ++i) {
        index *= 1103515245 + (hy_s32_t)key[i];
    }

    index >>= 27;
    index &= (bucket_max_len - 1);

    return index;
}

static hy_s32_t _find_item_from_list(_hash_context_t *context,
        HyHashItem_t *h_item, handle_item_cb_t handle_item_cb, hy_u32_t index)
{
    _item_t *pos;
    struct hy_hlist_node *n;
    hy_s32_t find_flag = -1;
    hy_u32_t key_hash = 0; 

    key_hash = HyHashGet(h_item->key);

    pthread_mutex_lock(&context->bucket_mutex[index]);
    hy_hlist_for_each_entry_safe(pos, n, &context->bucket_head[index], list) {
        if (pos->key_hash == key_hash) {
            find_flag = 0;

            if (handle_item_cb) {
                handle_item_cb(pos, h_item);
            }
            break;
        }
    }
    pthread_mutex_unlock(&context->bucket_mutex[index]);

    return find_flag;
}

static inline void _item_destroy(_item_t *item)
{
    HY_MEM_FREE_P(item->val);
    HY_MEM_FREE_P(item);
}

static _item_t *_item_init(HyHashItem_t *h_item)
{
    _item_t *item = NULL;
    do {
        item = HY_MEM_MALLOC_BREAK(_item_t *, sizeof(*item));
        item->val = HY_MEM_MALLOC_BREAK(void *, h_item->val_len);

        item->val_len = h_item->val_len;
        item->key_hash = HyHashGet(h_item->key);

        HY_MEMCPY(item->val, h_item->val, h_item->val_len);

        return item;
    } while (0);

    _item_destroy(item);
    return NULL;
}

static void _add_item_to_list(_hash_context_t *context, HyHashItem_t *h_item)
{
    LOGD("add item \n");

    hy_s32_t index = _key_to_index(context, h_item->key);
    _item_t *item  = _item_init(h_item);

    pthread_mutex_lock(&context->bucket_mutex[index]);
    hy_hlist_add_head(&item->list, &context->bucket_head[index]);
    pthread_mutex_unlock(&context->bucket_mutex[index]);
}

static inline void _replace_item_val(_item_t *pos, HyHashItem_t *h_item)
{
    LOGD("replace item \n");

    if (pos->val) {
        free(pos->val);
        pos->val = NULL;
    }

    pos->val = HY_MEM_MALLOC_RET(void *, h_item->val_len);

    HY_MEMCPY(pos->val, h_item->val, h_item->val_len);
}

static void _del_item_from_list(_item_t *pos, HyHashItem_t *h_item)
{
    LOGD("del item \n");

    hy_hlist_del(&pos->list);
    _item_destroy(pos);
}

static inline void _get_item_val(_item_t *pos, HyHashItem_t *h_item)
{
    LOGD("get item \n");

    h_item->val_len = pos->val_len;
    HY_MEMCPY(h_item->val, pos->val, pos->val_len);
}

hy_s32_t HyHashItemAdd(void *handle, HyHashItem_t *h_item)
{
    HY_ASSERT_RET_VAL(!handle || !h_item
            || !h_item->key || !h_item->val, -1);

    _hash_context_t *context = handle;
    hy_s32_t index;
    hy_s32_t find_flag;

    index = _key_to_index(context, h_item->key);
    find_flag = _find_item_from_list(context, h_item, _replace_item_val, index);

    if (-1 == find_flag) {
        _add_item_to_list(context, h_item);
    }

    return 0;
}

hy_s32_t HyHashItemDel(void *handle, HyHashItem_t *h_item)
{
    HY_ASSERT_RET_VAL(!handle || !h_item, -1);

    _hash_context_t *context = handle;
    hy_s32_t index = -1;

    index = _key_to_index(context, h_item->key);
    return _find_item_from_list(context, h_item, _del_item_from_list, index);
}

hy_s32_t HyHashItemGet(void *handle, HyHashItem_t *h_item)
{
    HY_ASSERT_RET_VAL(!handle || !h_item, -1);

    _hash_context_t *context = handle;
    hy_s32_t index = -1;

    index = _key_to_index(context, h_item->key);
    return _find_item_from_list(context, h_item, _get_item_val, index);
}

static void _traverse_item_list(_hash_context_t *context, hy_u32_t index,
        hy_s32_t type, HyHashDumpItemCb_t dump_item_cb, void *args)
{
    _item_t *pos;
    struct hy_hlist_node *n;

    pthread_mutex_lock(&context->bucket_mutex[index]);
    hy_hlist_for_each_entry_safe(pos, n, &context->bucket_head[index], list) {
        if (dump_item_cb) {
            if (type) {
                dump_item_cb(pos->val, args);
            } else {
                dump_item_cb(pos, args);
            }
        }
    }
    pthread_mutex_unlock(&context->bucket_mutex[index]);
}

void HyHashDump(void *handle, HyHashDumpItemCb_t dump_item_cb, void *args)
{
    HY_ASSERT_RET(!handle || !dump_item_cb);

    _hash_context_t *context = handle;

    for (hy_u32_t i = 0; i < context->save_config.bucket_cnt; i++) {
        LOGD("index: %d \n", i);
        _traverse_item_list(context, i, 1, dump_item_cb, args);
    }
}

static void _destroy_item_from_list(void *val, void *args)
{
    _del_item_from_list((_item_t *)val, (HyHashItem_t *)args);
}

void HyHashDestroy(void **handle)
{
    HY_ASSERT_RET(!handle || !*handle);

    _hash_context_t *context = *handle;

    for (size_t i = 0; i < context->save_config.bucket_cnt; i++) {
        _traverse_item_list(context, i, 0, _destroy_item_from_list, NULL);

        pthread_mutex_destroy(&context->bucket_mutex[i]);
    }

    HY_MEM_FREE_PP(&context->bucket_mutex);

    HY_MEM_FREE_PP(&context->bucket_head);

    HY_MEM_FREE_PP(handle);

    LOGI("hash destroy successful \n");
}

void *HyHashCreate(HyHashConfig_t *config)
{
    HY_ASSERT_RET_VAL(!config, NULL);

    size_t bucket_cnt;
    _hash_context_t *context = NULL;
    do {
        bucket_cnt = config->save_config.bucket_cnt;

        context = HY_MEM_MALLOC_BREAK(_hash_context_t *, sizeof(*context));
        context->bucket_head = HY_MEM_MALLOC_BREAK(struct hy_hlist_head *, bucket_cnt * sizeof(struct hy_hlist_head));
        context->bucket_mutex = HY_MEM_MALLOC_BREAK(pthread_mutex_t *, bucket_cnt * sizeof(pthread_mutex_t));

        HY_MEMCPY(&context->save_config, &config->save_config, sizeof(config->save_config));

        for (size_t i = 0; i < bucket_cnt; i++) {
            if (0 != pthread_mutex_init(&context->bucket_mutex[i], NULL)) {
                LOGE("pthread_mutex_init failed \n");
                goto _ERR_CREATE_1;
            }

            HY_INIT_HLIST_HEAD(&context->bucket_head[i]);
        }

        LOGI("hash create successful \n");
        return context;
    } while (0);

_ERR_CREATE_1:
    HyHashDestroy((void **)&context);
    return NULL;
}
