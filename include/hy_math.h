/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_math.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 14:08
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 14:08
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_MATH_H_
#define __LIBHY_UTILS_INCLUDE_HY_MATH_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 32位无符号最小值
 */
#ifndef HY_UINT32_MIN
#define HY_UINT32_MIN (0x00000000)
#endif /* ifndef HY_UINT32_MIN */

/**
 * @brief 32位无符号最大值
 */
#ifndef HY_UINT32_MAX
#define HY_UINT32_MAX (0xffffffff)
#endif /* ifndef HY_UINT32_MAX */

/**
 * @brief 32位有符号最小值
 */
#ifndef HY_INT32_MIN
#define HY_INT32_MIN (-2147483647 - 1) /**< 0x80000000 */
#endif /* ifndef HY_INT32_MIN */

/**
 * @brief 32位有符号最大值
 */
#ifndef HY_INT32_MAX
#define HY_INT32_MAX (0x7fffffff)
#endif /* ifndef HY_INT32_MAX */

/**
 * @brief 判断x是否是2的次方
 */
#define IS_POWER_OF_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))

#ifdef __cplusplus
}
#endif

#endif

