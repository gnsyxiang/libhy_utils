/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_utils.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 10:55
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 10:55
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_UTILS_H_
#define __LIBHY_UTILS_INCLUDE_HY_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#include "hy_hal/hy_type.h"

#define IP_INT_DOT_LEN (4)
#define IP_STR_DOT_LEN (16)

#define HY_UTILS_MIN(x, y)              ((x) < (y) ? (x) : (y))                             ///< 求最小值
#define HY_UTILS_MAX(x, y)              ((x) > (y) ? (x) : (y))                             ///< 求最大值

#define HY_UTILS_ROUND_UP(x, size)      ((((x) + size - 1) / size ) * size)                 ///< 对size向上取整

#define HY_UTILS_UPCASE(c)              (((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c))   ///< 将一个字母转换为大写

#define HY_UTILS_ARRAY_CNT(array)       (hy_u32_t)(sizeof((array)) / sizeof((array)[0]))    ///< 求数组元素的个数

#define HY_UTILS_IS_POWER_OF_2(x)       ((x) != 0 && (((x) & ((x) - 1)) == 0))              ///< 判断x是否为2^n，是返回1，否返回0

/**
 * @brief 把num转成2^n幂
 *
 * @param num 数字
 *
 * @return 对齐到2^n幂的数字
 */
static inline hy_u32_t HyUtilsNumTo2N(hy_u32_t num)
{
    hy_u32_t i = 1;
    hy_u32_t num_tmp = num;

    while (num >>= 1) {
        i <<= 1;
    }

    return (i < num_tmp) ? i << 1U : i;
}

/**
 * @brief 把IP字符串转化为整数
 *
 * @param ip_str ip字符串
 * @param ip_num ip整数
 */
void HyUtilsIpStr2Int(const char *ip_str, hy_u32_t *ip_num);

/**
 * @brief 把ip整数转化为ip字符串
 *
 * @param ip_num ip整数
 * @param ip_str ip字符串
 * @param ip_str_len ip字符串的长度
 */
void HyUtilsIpInt2Str(hy_u32_t ip_num, char *ip_str, size_t ip_str_len);

/**
 * @brief hex转化成整数，然后再转化成字符串
 *
 * @param addr 需要转换的hex数据地址
 * @param addr_len 需要转化的长度
 * @param str 转化成功后的字符串
 * @param str_len 字符串的长度
 */
void HyUtilsHex2Int2Str(char *addr, size_t addr_len, char *str, size_t str_len);

/**
 * @brief 字符串转化成整数，然后在转化成hex
 *
 * @param str 需要转化的字符串
 * @param str_len 字符串的长度
 * @param addr 转化成功后的hex数据地址
 * @param addr_len hex数据地址的长度
 */
void HyUtilsStr2Int2Hex(char *str, size_t str_len, char *addr, size_t addr_len);

/**
 * @brief 把比特数组转化成整数
 *
 * @param binary_str 比特数组
 * @param len 长度
 *
 * @return 返回整数
 */
hy_u32_t HyUtilsBitStr2Dec(char *bit_str, size_t len);

/**
 * @brief 把整数转化成bit数组
 *
 * @param num 整数
 * @param num_len 整数个数
 * @param bit_str bit数组地址
 * @param str_len bit数组长度
 */
void HyUtilsDec2BitStr(hy_u32_t num, size_t num_len,
        char *bit_str, size_t str_len);

#ifdef __cplusplus
}
#endif

#endif
