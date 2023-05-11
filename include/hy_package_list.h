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
    void                    *user_data;                 ///< 用户数据
    struct hy_list_head     entry;                      ///< 链表索引
} HyPackageListNode_s;

/**
 * @brief 创建用户数据回调函数
 *
 * @return 返回node节点，详见HyPackageListNode_s
 */
typedef HyPackageListNode_s *(*HyPackageListNodeCreateCb_t)(void);

/**
 * @brief 销毁用户数据回调函数
 *
 * @param list_node_pp 节点地址（二级指针）
 */
typedef void (*HyPackageListNodeDestroyCb_t)(HyPackageListNode_s **list_node_pp);

/**
 * @brief 配置参数
 */
typedef struct {
    hy_u32_t                        num;                ///< 创建链表的个数
    HyPackageListNodeCreateCb_t     node_create_cb;     ///< 创建用户数据回调函数
    HyPackageListNodeDestroyCb_t    node_destroy_cb;    ///< 销毁用户数据回调函数
} HyPackageListSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyPackageListSaveConfig_s       save_c;             ///< 配置参数
} HyPackageListConfig_s;

typedef struct HyPackageList_s HyPackageList_s;

/**
 * @brief 创建package_list模块
 *
 * @param package_list_c 配置参数，详见HyPackageListConfig_s
 * @return 成功返回句柄，失败返回NULL
 */
HyPackageList_s *HyPackageListCreate(HyPackageListConfig_s *package_list_c);

/**
 * @brief 销毁package_list模块
 *
 * @param handle_pp 句柄地址（二级指针）
 */
void HyPackageListDestroy(HyPackageList_s **handle_pp);

/**
 * @brief 从package_list中获取节点
 *
 * @param handle 句柄，详见HyPackageList_s
 * @return 返回node节点，详见HyPackageListNode_s
 */
HyPackageListNode_s *HyPackageListHeadGet(HyPackageList_s *handle);

/**
 * @brief 向package_list中加入node
 *
 * @param handle 句柄，详见HyPackageList_s
 * @param node node节点，详见HyPackageListNode_s
 */
void HyPackageListTailPut(HyPackageList_s *handle, HyPackageListNode_s *node);

/**
 * @brief 获取package_list中包含的个数
 *
 * @param handle 句柄，详见HyPackageList_s
 * @return 返回package_list中链表的个数
 */
hy_u32_t HyPackageListGetNodeCount(HyPackageList_s *handle);

#ifdef __cplusplus
}
#endif

#endif
