/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_package_list.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/04 2023 11:30
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/04 2023      create the file
 * 
 *     last modified: 21/04 2023 11:30
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_PACKAGE_LIST_H_
#define __LIBHY_UTILS_INCLUDE_HY_PACKAGE_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "hy_type.h"
#include "hy_list.h"

/**
 * @brief 链表节点
 */
typedef struct {
    void                    *user_data;             ///< 用户数据
    struct hy_list_head     entry;                  ///< 链表索引
} HyPackageListNode_s;

/**
 * @brief 创建用户数据回调函数
 *
 * @return 返回node节点，详见HyPackageListNode_s
 */
typedef HyPackageListNode_s *(*HyPackageListNodeCreate_t)(void);

/**
 * @brief 销毁用户数据回调函数
 *
 * @param node node节点
 */
typedef void (*HyPackageListNodeDestroy_t)(HyPackageListNode_s *node);

/**
 * @brief 配置参数
 */
typedef struct {
    hy_u32_t                    num;                ///< 创建链表的个数
    HyPackageListNodeCreate_t   node_create_cb;     ///< 创建用户数据回调函数
    HyPackageListNodeDestroy_t  node_destroy_cb;    ///< 销毁用户数据回调函数
} HyPackageListSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyPackageListSaveConfig_s   save_c;             ///< 配置参数
} HyPackageListConfig_s;

typedef struct HyPackageList_s HyPackageList_s;

/**
 * @brief 创建package_list模块
 *
 * @param config 配置参数，详见
 * @return 返回package_list链表
 */
HyPackageList_s *HyPackageListCreate(HyPackageListConfig_s *config);

/**
 * @brief 销毁package_list模块
 *
 * @param context_pp 链表地址（二级指针）
 */
void HyPackageListDestroy(HyPackageList_s **context_pp);

/**
 * @brief 从package_list中获取节点
 *
 * @param context 链表，详见HyPackageList_s
 * @return 返回node节点，详见HyPackageListNode_s
 */
HyPackageListNode_s *HyPackageListHeadGet(HyPackageList_s *context);

/**
 * @brief 向package_list中加入node
 *
 * @param context 链表，详见HyPackageList_s
 * @param node node节点，详见HyPackageListNode_s
 */
void HyPackageListTailPut(HyPackageList_s *context, HyPackageListNode_s *node);

/**
 * @brief 获取package_list中包含的个数
 *
 * @param context 链表，详见HyPackageList_s
 * @return 返回package_list中链表的个数
 */
hy_u32_t HyPackageListGetNodeCount(HyPackageList_s *context);

#ifdef __cplusplus
}
#endif

#endif

