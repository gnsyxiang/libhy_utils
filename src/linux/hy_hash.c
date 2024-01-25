/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_hash.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    19/05 2023 16:16
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        19/05 2023      create the file
 * 
 *     last modified: 19/05 2023 16:16
 */
#include <stdio.h>

#include <hy_os_type/hy_assert.h>
#include <hy_os_type/hy_mem.h>
#include <hy_os_type/hy_string.h>
#include <hy_os_type/hy_list.h>

#include <hy_os/hy_thread_mutex.h>

#include "hy_hash.h"

typedef struct {
    HyHashItem_s            item;

    struct hy_hlist_node    entry;
} _list_item_s;

struct HyHash_s {
    HyHashSaveConfig_s      save_c;

    HyThreadMutex_s         **list_mutex_h;
    struct hy_hlist_head    *list;
};

typedef void (*handle_item_cb_t)(_list_item_s *pos, HyHashItem_s *item);

hy_u32_t _hash_get(const char *key)
{
    hy_u32_t seed = 31; // 31 131 1313 13131 131313 etc..
    hy_u32_t hash = 0;

    while (*key) {
        hash = hash * seed + (*key++);
    }

    return hash;
}

static hy_u32_t _key_2_index(hy_u32_t bucket_cnt, const char *key)
{
    hy_u32_t len = HY_STRLEN(key);
    hy_s32_t index = (hy_s32_t)key[0];

    for (hy_u32_t i = 0; i < len; i++) {
        // index *= 1103515245 + (hy_s32_t)key[i];
        index *= 0x41C64E6D + (hy_s32_t)key[i];
    }

    index >>= 27;
    index &= (bucket_cnt - 1);

    return index;
}

static void _list_item_destroy(_list_item_s **list_item_pp)
{
    _list_item_s *list_item = *list_item_pp;

    HY_MEM_FREE_PP(&list_item->item.val);

    LOGI("list item create, list_item: %p \n", list_item);
    HY_MEM_FREE_PP(list_item_pp);
}

static _list_item_s *_list_item_create(HyHashItem_s *item)
{
    _list_item_s *list_item = NULL;
    do {
        list_item = HY_MEM_CALLOC_BREAK(_list_item_s *, sizeof(*list_item));
        list_item->item.val = HY_MEM_CALLOC_BREAK(void *, item->val_len);

        list_item->item.val_len = item->val_len;
        list_item->item.key_hash = _hash_get(item->key);

        HY_MEMCPY(list_item->item.val, item->val, item->val_len);

        LOGI("list item create, list_item: %p \n", list_item);
        return list_item;
    } while(0);

    LOGE("list item create failed \n");
    _list_item_destroy(&list_item);
    return NULL;
}

static void _item_val_replace(_list_item_s *pos, HyHashItem_s *item)
{
    LOGD("replace item \n");

    HY_MEM_FREE_PP(&pos->item.val);

    pos->item.val = HY_MEM_MALLOC_RET(void *, item->val_len);

    HY_MEMCPY(pos->item.val, item->val, item->val_len);
}

static hy_s32_t _item_val_add(HyHash_s *handle, HyHashItem_s *item)
{
    LOGD("add item \n");

    hy_s32_t index = _key_2_index(handle->save_c.bucket_cnt, item->key);
    _list_item_s *list_item  = _list_item_create(item);
    if (!list_item) {
        LOGE("_list_item_create failed \n");
        return -1;
    }

    HyThreadMutexLock(handle->list_mutex_h[index]);
    hy_hlist_add_head(&list_item->entry, &handle->list[index]);
    HyThreadMutexUnLock(handle->list_mutex_h[index]);

    return 0;
}

static void _item_val_del(_list_item_s *pos, HyHashItem_s *item)
{
    LOGD("del item \n");

    hy_hlist_del(&pos->entry);

    if (item) {
        HY_MEM_FREE_PP(&item->val);
        item->val = HY_MEM_CALLOC_RETURN(void *, pos->item.val_len);

        item->val_len = pos->item.val_len;
        HY_MEMCPY(item->val, pos->item.val, pos->item.val_len);
    }

    _list_item_destroy(&pos);
}

static void _item_val_get(_list_item_s *pos, HyHashItem_s *item)
{
    LOGD("get item \n");

    item->val_len = pos->item.val_len;
    HY_MEMCPY(item->val, pos->item.val, pos->item.val_len);
}

static hy_s32_t _list_item_find(HyHash_s *handle, hy_u32_t index,
                                handle_item_cb_t handle_item_cb,
                                HyHashItem_s *item)
{
    _list_item_s *pos;
    struct hy_hlist_node *n;
    hy_s32_t find_flag = 0;
    hy_u32_t key_hash = 0; 

    LOGI("find list item \n");

    key_hash = _hash_get(item->key);

    HyThreadMutexLock(handle->list_mutex_h[index]);
    hy_hlist_for_each_entry_safe(pos, n, &handle->list[index], entry) {
        if (pos->item.key_hash == key_hash) {
            find_flag = 1;

            if (handle_item_cb) {
                handle_item_cb(pos, item);
            }
            break;
        }
    }
    HyThreadMutexUnLock(handle->list_mutex_h[index]);

    return find_flag;
}

hy_s32_t HyHashAdd(HyHash_s *handle, HyHashItem_s *item)
{
    HY_ASSERT_RET_VAL(!handle || !item, -1);
    hy_u32_t index;
    hy_s32_t find_flag;

    index = _key_2_index(handle->save_c.bucket_cnt, item->key);
    find_flag = _list_item_find(handle, index, _item_val_replace, item);

    if (0 == find_flag) {
        find_flag = _item_val_add(handle, item);
    }

    return (find_flag == -1 ? -1 : 0);
}

hy_s32_t HyHashPeekGet(HyHash_s *handle, HyHashItem_s *item)
{
    HY_ASSERT_RET_VAL(!handle || !item, -1);
    hy_u32_t index;
    hy_s32_t find_flag;

    index = _key_2_index(handle->save_c.bucket_cnt, item->key);
    find_flag = _list_item_find(handle, index, _item_val_get, item);

    return (find_flag == 1 ? 0 : -1);
}

hy_s32_t HyHashDel(HyHash_s *handle, HyHashItem_s *item)
{
    HY_ASSERT_RET_VAL(!handle || !item, -1);
    hy_u32_t index;
    hy_s32_t find_flag;

    index = _key_2_index(handle->save_c.bucket_cnt, item->key);
    find_flag = _list_item_find(handle, index, _item_val_del, item);

    return (find_flag == 1 ? 0 : -1);
}

static void _traverse_item_list(HyHash_s *handle,
                                hy_u32_t index, hy_s32_t type,
                                HyHashDumpItemCb_t dump_item_cb, void *args)
{
    _list_item_s *pos;
    struct hy_hlist_node *n;

    HyThreadMutexLock(handle->list_mutex_h[index]);
    hy_hlist_for_each_entry_safe(pos, n, &handle->list[index], entry) {
        if (dump_item_cb) {
            if (type) {
                dump_item_cb(&pos->item, args);
            } else {
                // NOTE: 为了对外接口而进行的强制类型转换
                dump_item_cb((HyHashItem_s *)pos, args);
            }
        }
    }
    HyThreadMutexUnLock(handle->list_mutex_h[index]);
}

void HyHashDump(HyHash_s *handle, const char *key,
                HyHashDumpItemCb_t dump_item_cb, void *args)
{
    hy_u32_t index;
    _list_item_s *pos;
    struct hy_hlist_node *n;
    hy_s32_t flag = 0;

    index = _key_2_index(handle->save_c.bucket_cnt, key);

    HyThreadMutexLock(handle->list_mutex_h[index]);
    hy_hlist_for_each_entry_safe(pos, n, &handle->list[index], entry) {
        if (dump_item_cb) {
            dump_item_cb(&pos->item, args);
            flag = 1;
            break;
        }
    }
    HyThreadMutexUnLock(handle->list_mutex_h[index]);

    if (!flag) {
        LOGI("don't find %s \n", key);
    }
}

void HyHashDumpAll(HyHash_s *handle, HyHashDumpItemCb_t dump_item_cb, void *args)
{
    HY_ASSERT_RET(!handle || !dump_item_cb);
    LOGI("hash dump all: \n");

    for (hy_u32_t i = 0; i < handle->save_c.bucket_cnt; i++) {
        _traverse_item_list(handle, i, 1, dump_item_cb, args);
    }
}

static void _destroy_item_from_list(HyHashItem_s *list_item, void *args)
{
    // NOTE: 为了对外接口而进行的强制类型转换，跟上面是同一个注意事项
    _item_val_del((_list_item_s *)list_item, NULL);
}

void HyHashDestroy(HyHash_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyHash_s *handle = *handle_pp;

    for (hy_u32_t i = 0; i < handle->save_c.bucket_cnt; i++) {
        _traverse_item_list(handle, i, 0, _destroy_item_from_list, NULL);

        HyThreadMutexDestroy(&handle->list_mutex_h[i]);
    }

    HY_MEM_FREE_PP(&handle->list_mutex_h);
    HY_MEM_FREE_PP(&handle->list);

    LOGI("hash destroy successful, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyHash_s *HyHashCreate(HyHashConfig_s *hash_c)
{
    HY_ASSERT_RET_VAL(!hash_c, NULL);
    HyHash_s *handle = NULL;
    hy_u32_t bucket_cnt;

    do {
        handle = HY_MEM_CALLOC_BREAK(HyHash_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &hash_c->save_c, sizeof(hash_c->save_c));

        bucket_cnt = hash_c->save_c.bucket_cnt;

        handle->list = HY_MEM_CALLOC_BREAK(struct hy_hlist_head *, bucket_cnt * sizeof(struct hy_hlist_head));
        handle->list_mutex_h = HY_MEM_CALLOC_BREAK(HyThreadMutex_s **, bucket_cnt * sizeof(HyThreadMutex_s *));

        for (hy_u32_t i = 0; i < bucket_cnt; i++) {
            handle->list_mutex_h[i] = HyThreadMutexCreate_m();
            if (!handle->list_mutex_h[i]) {
                LOGE("HyThreadMutexCreate_m failed \n");
                break;
            }

            HY_INIT_HLIST_HEAD(&handle->list[i]);
        }

        LOGI("hash create successful, handle: %p \n", handle);
        return handle;
    } while(0);

    LOGE("hash create failed \n");
    return NULL;
}
