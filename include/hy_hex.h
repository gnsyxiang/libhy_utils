/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_hex.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    12/04 2022 10:20
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        12/04 2022      create the file
 * 
 *     last modified: 12/04 2022 10:20
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_HEX_H_
#define __LIBHY_UTILS_INCLUDE_HY_HEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_type.h"

/**
 * @brief 打印二进制bit信息
 *
 * @param buf 数据
 * @param len 数据长度
 * @param out_buf 输出数据
 * @param out_len 输出数据长度
 */
void HyHexBit(const void *buf, hy_u32_t len, char *out_buf, hy_u32_t out_len);

/**
 * @brief 打印二进制bit信息宏
 *
 * @param _buf 数据
 * @param _len 长度
 */
#define HY_HEX_BIT(_buf, _len)                      \
do {                                                \
    char __buf[1024] = {0};                         \
    HyHexBit(_buf, _len, __buf, sizeof(__buf));     \
    LOGI("%s\n", __buf);                            \
} while (0)

/**
 * @brief 打印二进制信息
 *
 * @param buf 数据
 * @param len 数据长度
 * @param out_buf 输出数据
 * @param out_len 输出数据长度
 * @param flag 是否打印ASCII，为1时打印，为0时不打印
 *
 * @return 返回写入的字节数
 */

hy_u32_t HyHex(const void *buf, hy_u32_t len,
               char *out_buf, hy_u32_t out_len, hy_s32_t flag);

/**
 * @brief 打印二进制信息宏，带ASCII码
 *
 * @param _buf 数据
 * @param _len 长度
 *
 * @note 注意接收__buf大小，否则打印不全
 */
#define HY_HEX_ASCII(_buf, _len)                            \
do {                                                        \
    char *__buf = HY_MEM_CALLOC_BREAK(char *, 10 * _len);   \
    HyHex(_buf, _len, __buf, 10 * _len, 1);                 \
    LOGI("len: %d \n%s\n", (hy_u32_t)_len, __buf);          \
    HY_MEM_FREE_PP(&__buf);                                 \
} while (0)

/**
 * @brief 打印二进制信息宏
 *
 * @param _buf 数据
 * @param _len 长度
 *
 * @note 注意接收__buf大小，否则打印不全
 */
#define HY_HEX(_buf, _len)                                  \
do {                                                        \
    char *__buf = HY_MEM_CALLOC_BREAK(char *, 5 * _len);    \
    HyHex(_buf, _len, __buf, 5 * _len, 1);                  \
    LOGI("len: %d \n%s\n", (hy_u32_t)_len, __buf);          \
    HY_MEM_FREE_PP(&__buf);                                 \
} while (0)

#ifdef __cplusplus
}
#endif

#endif

