/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    dynamic_array.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    22/04 2022 09:04
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        22/04 2022      create the file
 * 
 *     last modified: 22/04 2022 09:04
 */
#ifndef __LIBHY_UTILS_INCLUDE_DYNAMIC_ARRAY_H_
#define __LIBHY_UTILS_INCLUDE_DYNAMIC_ARRAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_type.h"

typedef struct HyDynamicArray_s HyDynamicArray_s;

/**
 * @brief 创建动态数组
 *
 * @param min_len 最小长度
 * @param max_len 最大长度
 *
 * @return 成功返回句柄，失败返回NULL
 */
HyDynamicArray_s *HyDynamicArrayCreate(hy_u32_t min_len, hy_u32_t max_len);

/**
 * @brief 销毁动态数组
 *
 * @param handle_pp 句柄的地址（二级指针）
 */
void HyDynamicArrayDestroy(HyDynamicArray_s **handle_pp);

/**
* @brief 复位动态数据
*
* @param handle 句柄
*/
void HyDynamicArrayReset(HyDynamicArray_s *handle);

/**
 * @brief 从动态数组中读取
 *
 * @param handle 句柄
 * @param buf 数组
 * @param len 长度
 *
 * @return 返回实际读取到的长度
 */
hy_s32_t HyDynamicArrayRead(HyDynamicArray_s *handle, void *buf, hy_u32_t len);

/**
 * @brief 向动态数组中写入
 *
 * @param handle 句柄
 * @param buf 数组
 * @param len 长度
 *
 * @return 成功返回写入的长度，失败返回-1
 */
hy_s32_t HyDynamicArrayWrite(HyDynamicArray_s *handle, const void *buf, hy_u32_t len);

/**
 * @brief 向动态数组中写入
 *
 * @param handle 句柄
 * @param fmt 用户格式
 * @param args 用户数据
 *
 * @return 成功返回写入的长度，失败返回-1
 */
hy_s32_t HyDynamicArrayWriteVprintf(HyDynamicArray_s *handle, const char *fmt, va_list *args);


#ifdef __cplusplus
}
#endif

#endif

