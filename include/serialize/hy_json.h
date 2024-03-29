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

#include <hy_os_type/hy_type.h>

#if 0
/**
 * @brief json文件句柄
 *
 */
typedef struct {
    void        *root;          ///< json的root节点
    const char  *file;          ///< 文件路径
    hy_s32_t    save_flag;      ///< 是否保存到文件，1为保存，0为不保存
} HyJsonFile_s;
#endif

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
 * @brief 销毁json
 *
 * @param root json节点
 */
void HyJsonDestroy(void *root);

/**
 * @brief 创建json节点
 *
 * @return 成功返回json节点，失败返回NULL
 */
void *HyJsonCreate(void);

/**
 * @brief 从buf中创建json
 *
 * @param buf json字符串
 *
 * @return 成功返回root节点，失败返回NULL
 */
void *HyJsonCreateFromBuf(const char *buf);

/**
 * @brief 从文件中创建json
 *
 * @param file 文件的路径
 *
 * @return 成功返回root节点，失败返回NULL
 */
void *HyJsonCreateFromFile(const char *file);

/**
 * @brief 创建json数组节点
 *
 * @return 成功返回json节点，失败返回NULL
 */
void *HyJsonArrayNew(void);

/**
 * @brief 向json数组加入元素
 *
 * @param array json数组
 * @param item json元素
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyJsonArrayAdd(void *array, void *item);

/**
 * @brief 从整数创建json节点
 *
 * @param val 整数数值
 *
 * @return 成功返回json节点，失败返回NULL
 */
void *HyJsonFromInt(hy_s64_t val);

/**
 * @brief 从浮点数创建json节点
 *
 * @param val 浮点数
 *
 * @return 成功返回json节点，失败返回NULL
 */
void *HyJsonFromReal(hy_double_t val);

/**
 * @brief 从字符串创建json节点
 *
 * @param val 字符串
 *
 * @return 成功返回json节点，失败返回NULL
 */
void *HyJsonFromStr(const char *val);

/**
 * @brief 向指定域中加入整数值
 *
 * @param root json节点
 * @param field 指定域名称
 * @param val 整数值
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyJsonAddInt(void *root, const char *field, hy_s64_t val);

/**
 * @brief 向指定域中加入浮点数
 *
 * @param root json节点
 * @param field 指定域名称
 * @param val 浮点数
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyJsonAddReal(void *root, const char *field, hy_double_t val);

/**
 * @brief 向指定域中加入字符串
 *
 * @param root json节点
 * @param field 指定域名称
 * @param val 字符串
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyJsonAddStr(void *root, const char *field, const char *val);

/**
 * @brief 向指定域中加入json节点
 *
 * @param root json节点
 * @param field 指定域名称
 * @param item 需要加入的json节点
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyJsonAddObject(void *root, const char *field, void *item);

/**
 * @brief 打印root中的信息
 *
 * @param root root根节点
 *
 * @return 成功返回需要打印的内容，失败返回NULL
 *
 * @note 成功返回内容后，需要用户释放内容的空间
 */
char *HyJsonDump(void *root);

#if (HY_JSON_USE_TYPE == 1)
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
 * @brief 根据可变参数获取item，然后操作
 *
 * @param type 操作类型，0为获取，1为设置
 * @param val 值
 * @param root json根节点
 * @param n 可变参数的个数
 * @param ... 可变参数
 *
 * @return
 * 当type为0，成功返回对应值，失败返回val
 * 当type为1，成功返回0，失败返回-1
 */
hy_s32_t HyJsonOperItemInt_va(hy_s32_t type, hy_s32_t val,
                              void *root, hy_s32_t n, ...);

/**
 * @brief 根据可变参数获取item的int值，失败返回error_val
 */
#define HyJsonGetItemInt(error_val, item, x...) \
    HyJsonOperItemInt_va(0, error_val, item, comac_argc(x), x)

/**
 * @brief 根据可变参数设置item的int值，失败返回-1
 */
#define HyJsonSetItemInt(val, item, x...) \
    HyJsonOperItemInt_va(1, val, item, comac_argc(x), x)

/**
 * @brief 根据可变参数获取item，然后操作
 *
 * @param type 操作类型，0为获取，1为设置
 * @param val 值
 * @param root json根节点
 * @param n 可变参数的个数
 * @param ... 可变参数
 *
 * @return
 * 当type为0，成功返回对应值，失败返回val
 * 当type为1，成功返回0，失败返回-1
 */
double HyJsonOperItemReal_va(hy_s32_t type, double val,
                             void *root, hy_s32_t n, ...);

/**
 * @brief 根据可变参数获取item的double值，失败返回error_val
 */
#define HyJsonGetItemReal(error_val, item, x...) \
    HyJsonOperItemReal_va(0, error_val, item, comac_argc(x), x)

/**
 * @brief 根据可变参数设置item的double值，失败返回-1
 */
#define HyJsonSetItemReal(val, item, x...) \
    HyJsonOperItemReal_va(1, val, item, comac_argc(x), x)

/**
 * @brief 根据可变参数获取char *值
 *
 * @param val 用户指定出错的返回值
 * @param root json根节点
 * @param n 参数个数
 * @param ... 可变参数
 *
 * @return 成功返回item中的值，失败返回用户指定的error_val
 */
const char *HyJsonGetItemStr_va(const char *val, void *root, hy_s32_t n, ...);

/**
 * @brief 根据可变参数获取item的char *值，失败返回error_val
 */
#define HyJsonGetItemStr(error_val, item, x...) \
    HyJsonGetItemStr_va(error_val, item, comac_argc(x), x)

/**
 * @brief 根据可变参数设置char *值
 *
 * @param val 值
 * @param root json根节点
 * @param n 参数个数
 * @param ... 可变参数
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyJsonSetItemStr_va(const char *val, void *root, hy_s32_t n, ...);

/**
 * @brief 根据可变参数设置item的char *值，失败返回-1
 */
#define HyJsonSetItemStr(val, root, x...) \
    HyJsonSetItemStr_va(val, root, comac_argc(x), x)

#elif (HY_JSON_USE_TYPE == 2)

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
hy_s32_t HyJsonGetItemInt2(hy_s32_t err_val, void *root,
                           char *fmt, hy_u32_t fmt_len);

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
double HyJsonGetItemReal2(double err_val, void *root,
                          char *fmt, hy_u32_t fmt_len);

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
                              void *root, char *fmt, hy_u32_t fmt_len);

#endif

#if 0
/**
 * @brief 从文件中创建json
 *
 * @param file 文件路径
 *
 * @return 成功返回句柄，失败返回NULL
 */
HyJsonFile_s *HyJsonFileCreate(const char *file);

/**
 * @brief 销毁从文件中创建json
 *
 * @param handle 句柄的地址（二级指针）
 */
void HyJsonFileDestroy(HyJsonFile_s **handle_pp);
#endif

#ifdef __cplusplus
}
#endif

#endif

