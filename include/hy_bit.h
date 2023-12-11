/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_bit.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 14:20
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 14:20
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_BIT_H_
#define __LIBHY_UTILS_INCLUDE_HY_BIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_log/hy_type.h>

/**
 * @brief 获取一个字节从0位到指定位的掩码
 *
 * @param _index 整数值(0，1, 2, 3, 4, 5, 6, 7)
 *
 * @return 返回值指定位的掩码(0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80)
 */
#define HY_BIT_MASK_8(_index)               ((hy_u8_t)(0xff << (_index)))

/**
 * @brief 获取一个字节从0位到指定位的掩码
 *
 * @param _index 整数值(0，1, 2, 3 ...)
 *
 * @return 返回值指定位的掩码
 */
#define HY_BIT_MASK_16(_index)              ((hy_u16_t)(0xffff << (_index)))

/**
 * @brief 获取一个字节从0位到指定位的掩码
 *
 * @param _index 整数值(0，1, 2, 3 ...)
 *
 * @return 返回值指定位的掩码
 */
#define HY_BIT_MASK_32(_index)              ((hy_u32_t)(0xffffffff << (_index)))

/**
 * @brief 获取一个字节从0位到指定位的掩码
 *
 * @param _index 整数值(0，1, 2, 3 ...)
 *
 * @return 返回值指定位的掩码
 */
#define HY_BIT_MASK_64(_index)              ((hy_u64_t)(0xffffffffffffffff << (_index)))

/**
 * @brief 向左移位操作
 *
 * @param _index 整数值(0，1, 2, 3, ...)
 *
 * @return 返回值(0x1, 0x2...)
 */
#define HY_BIT_SHIFT(_index)                (0x1ULL << (_index))

/**
 * @brief 在val中检查bit索引位是否置1
 *
 * @param _val 整数值
 * @param _bit_val 整数值(0x1, 0x2, 0x4, ...)
 *
 * @return 置1返回1，否者返回0
 */
#define HY_BIT_IS_SET(_val, _bit_val)       (((_val) & (_bit_val)) == (_bit_val))

/**
 * @brief 在val中检查bit索引位是否置0
 *
 * @param _val 整数值
 * @param _bit_val 整数值(0x1, 0x2, 0x4, ...)
 *
 * @return 置0返回1，否者返回0
 */
#define HY_BIT_IS_RESET(_val, _bit_val)     ((((_val) & (_bit_val)) ^ (_bit_val)) == (_bit_val))

/**
 * @brief 对应bit设置为1
 *
 * @param byte 数据
 * @param len 数据的长度
 * @param index 需要设置的bit，起始位为0，最高位为7
 *
 * @note 数据没有限制，可以是1个字节，2个字节，4个字节，8个字节，或者9个字节
 *       函数只是找到对应的字节，把相应bit设置为1
 */
void HyBitSet(char *byte, hy_u32_t len, hy_u32_t index);

/**
 * @brief 对应bit设置为0
 *
 * @param byte 数据
 * @param len 数据的长度
 * @param index 需要设置的bit，起始位为0，最高位为7
 *
 * @note 数据没有限制，可以是1个字节，2个字节，4个字节，8个字节，或者9个字节
 *       函数只是找到对应的字节，把相应bit设置为0
 */
void HyBitReSet(char *byte, hy_u32_t len, hy_u32_t index);

/**
 * @brief 获取对应bit
 *
 * @param byte 数据
 * @param len 数据的长度
 * @param index 需要设置的bit，起始位为0，最高位为7
 *
 * @return 成功返回0或1，失败返回-1
 */
hy_s32_t HyBitGet(char *byte, hy_u32_t len, hy_u32_t index);

#ifdef __cplusplus
}
#endif

#endif

