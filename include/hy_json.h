/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_json.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 19:39
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 19:39
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_JSON_H_
#define __LIBHY_UTILS_INCLUDE_HY_JSON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define HyJson_t void

/**
 * @brief 提供两种解析json的方法
 *
 * 方法1: 优点: 解析速度快，缺点: 提供的参数需要一个一个指定
 * 方法2: 优点: 参数一次性提供，缺点: 速度稍微慢一点并且会有堆空间的开辟与释放
 *
 * note: 优先选择方法1
 */
#define HY_JSON_USE_TYPE   (1)

/**
 * @brief 从buf中创建json
 *
 * @param buf json字符串
 *
 * @return 成功返回root节点，失败返回NULL
 */
HyJson_t *HyJsonCreate(const char *buf);

/**
 * @brief 销毁从buf中创建的json
 *
 * @param root root根节点
 */
void HyJsonDestroy(HyJson_t *root);

/**
 * @brief 打印root中的信息
 *
 * @param root root根节点
 */
void HyJsonDump(HyJson_t *root);

/**
 * @brief 从文件中创建json
 *
 * @param name 文件路径
 *
 * @return 成功返回root节点，失败返回NULL
 */
HyJson_t *HyJsonFileCreate(const char *name);

/**
 * @brief 销毁从文件中创建json
 *
 * @param root root根节点
 */
void HyJsonFileDestroy(HyJson_t *root);

#if (HY_JSON_USE_TYPE == 1)
#if 1
/**
 * @brief 计算参数的个数
 */
#define comac_args_seqs()                                   \
    39, 38, 37, 36, 35, 34, 33, 32, 31, 30,                 \
    29, 28, 27, 26, 25, 24, 23, 22, 21, 20,                 \
    19, 18, 17, 16, 15, 14, 13, 12, 11, 10,                 \
    9,   8,  7,  6,  5,  4,  3,  2,  1,  0

/**
 * @brief 计算参数的个数
 */
#define comac_arg_n(                                        \
        _0,   _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,   \
        _10, _11, _12, _13, _14, _15, _16, _17, _18, _19,   \
        _20, _21, _22, _23, _24, _25, _26, _27, _28, _29,   \
        _30, _31, _32, _33, _34, _35, _36, _37, _38, _39,   \
                                                            \
        _N, ...)                                            \
        _N

/**
 * @brief 计算参数的个数
 */
#define comac_get_args_cnt(...) comac_arg_n(__VA_ARGS__)

/**
 * @brief 计算参数的个数
 */
#define comac_argc(...) comac_get_args_cnt(0, ##__VA_ARGS__, comac_args_seqs())

/**
 * @brief 根据可变参数获取int值
 *
 * @param error_val 用户指定出错的返回值
 * @param root json根节点
 * @param n 参数个数
 * @param ... 可变参数
 *
 * @return 成功返回item中的值，失败返回用户指定的error_val
 */
int HyJsonGetItemInt_va(int error_val, HyJson_t *root, int n, ...);

/**
 * @brief 根据可变参数获取double值
 *
 * @param error_val 用户指定出错的返回值
 * @param root json根节点
 * @param n 参数个数
 * @param ... 可变参数
 *
 * @return 成功返回item中的值，失败返回用户指定的error_val
 */
double HyJsonGetItemReal_va(double error_val, HyJson_t *root, int n, ...);

/**
 * @brief 根据可变参数获取char *值
 *
 * @param error_val 用户指定出错的返回值
 * @param root json根节点
 * @param n 参数个数
 * @param ... 可变参数
 *
 * @return 成功返回item中的值，失败返回用户指定的error_val
 */
const char *HyJsonGetItemStr_va(const char *error_val, HyJson_t *root, int n, ...);
#endif

/**
 * @brief 获取item中的int值，详见HyJsonGetItemInt_va
 */
#define HyJsonGetItemInt(error_val, root, x...) \
    HyJsonGetItemInt_va(error_val, root, comac_argc(x), x)

/**
 * @brief 获取item中的double值，详见HyJsonGetItemReal_va
 */
#define HyJsonGetItemReal(error_val, root, x...) \
    HyJsonGetItemReal_va(error_val, root, comac_argc(x), x)

/**
 * @brief 获取item中的char *值，详见HyJsonGetItemStr_va
 */
#define HyJsonGetItemStr(error_val, root, x...) \
    HyJsonGetItemStr_va(error_val, root, comac_argc(x), x)

#endif

#if (HY_JSON_USE_TYPE == 2)

/**
 * @brief 获取item中的int值
 *
 * @param err_val 用户指定出错的返回值
 * @param root json根节点
 * @param fmt 获取指定内容的字符串
 * @param fmt_len 字符串的长度
 *
 * @return 成功返回item中的值，失败返回用户指定的error_val
 */
int HyJsonGetItemInt2(int err_val, HyJson_t *root, char *fmt, size_t fmt_len);

/**
 * @brief 根据可变参数获取double值
 *
 * @param error_val 用户指定出错的返回值
 * @param root json根节点
 * @param fmt 获取指定内容的字符串
 * @param fmt_len 字符串的长度
 *
 * @return 成功返回item中的值，失败返回用户指定的error_val
 */
double HyJsonGetItemReal2(double err_val,
        HyJson_t *root, char *fmt, size_t fmt_len);

/**
 * @brief 根据可变参数获取char *值
 *
 * @param error_val 用户指定出错的返回值
 * @param root json根节点
 * @param fmt 获取指定内容的字符串
 * @param fmt_len 字符串的长度
 *
 * @return 成功返回item中的值，失败返回用户指定的error_val
 */
const char *HyJsonGetItemStr2(const char *err_val,
        HyJson_t *root, char *fmt, size_t fmt_len);

#endif

#ifdef __cplusplus
}
#endif

#endif

