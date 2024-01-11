/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_hash.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    19/05 2023 15:58
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        19/05 2023      create the file
 * 
 *     last modified: 19/05 2023 15:58
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_HASH_H_
#define __LIBHY_UTILS_INCLUDE_HY_HASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_os_type/hy_os_type.h>

/**
 * @brief item结构体
 */
typedef struct {
    const char  *key;           ///< 关键词
    hy_u32_t    key_hash;       ///< 关键词转换后的数字，供内部使用

    void        *val;           ///< 关键词对应的val
    hy_u32_t    val_len;        ///< val长度
} HyHashItem_s;

/**
 * @brief 打印item回调函数
 */
typedef void (*HyHashDumpItemCb_t)(HyHashItem_s *item, void *args);

/**
 * @brief 配置参数
 */
typedef struct {
    hy_u32_t            bucket_cnt;     ///< 桶的个数，即数组的总长度
} HyHashSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyHashSaveConfig_s  save_c;         ///< 配置参数
} HyHashConfig_s;

typedef struct HyHash_s HyHash_s;

/**
 * @brief 创建hash模块
 *
 * @param hash_c 配置参数，详见HyHashConfig_s
 * @return 成功返回句柄，失败返回NULL
 */
HyHash_s *HyHashCreate(HyHashConfig_s *hash_c);

/**
 * @brief 销毁hash模块
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyHashDestroy(HyHash_s **handle_pp);

/**
 * @brief 向hash模块中增加元素
 *
 * @param handle 句柄
 * @param item 新增的元素
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyHashAdd(HyHash_s *handle, HyHashItem_s *item);

/**
 * @brief 从hash模块中删除元素
 *
 * @param handle 句柄
 * @param item 元素
 * @return 成功返回0，失败返回-1
 *
 * @note 函数中会开辟内存空间(item->val)，需要用户释放，否则造成内存泄漏
 */
hy_s32_t HyHashDel(HyHash_s *handle, HyHashItem_s *item);

/**
 * @brief 从hash模块中获取指定元素的信息
 *
 * @param handle 句柄
 * @param item 元素
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyHashPeekGet(HyHash_s *handle, HyHashItem_s *item);

/**
 * @brief 打印hash模块中指定元素的信息
 *
 * @param handle 句柄
 * @param key 索引
 * @param dump_item_cb 回调函数
 * @param args 用户参数
 */
void HyHashDump(HyHash_s *handle, const char *key,
                HyHashDumpItemCb_t dump_item_cb, void *args);

/**
 * @brief 打印hash模块中的所有元素信息
 *
 * @param handle 句柄
 * @param dump_item_cb 回调函数
 * @param args 用户参数
 */
void HyHashDumpAll(HyHash_s *handle,
                   HyHashDumpItemCb_t dump_item_cb, void *args);

#ifdef __cplusplus
}
#endif

#endif

