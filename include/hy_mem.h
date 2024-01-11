/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_mem.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    25/10 2021 19:11
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        25/10 2021      create the file
 * 
 *     last modified: 25/10 2021 19:11
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_MEM_H_
#define __LIBHY_UTILS_INCLUDE_HY_MEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hy_os_type/hy_os_type.h>

#define HY_MEM_BYTE(x)                          (*((unsigned char *)(x)))                                       ///< 得到指定地址上的一个字节
#define HY_MEM_WORD(x)                          (*((unsigned short *)(x)))                                      ///< 得到指定地址上的一个字
#define HY_MEM_DOUBLE_WORD(x)                   (*((unsigned int *)(x)))                                        ///< 得到指定地址上的一个双字

#define HY_MEM_OFFSETOF(_type, _member)         ((hy_s32_t) &((_type *)0)->_member)                             ///< 得到一个member在结构体(struct)中的偏移量
#define HY_MEM_OFFSETOF_SIZE(_type, _member)    sizeof(((_type *)0)->_member)                                   ///< 得到一个结构体中member所占用的字节数

#define HY_MEM_LSB_BYTE_2_WORD(_array)          ((((unsigned short) (_array)[0]) << 8) + (_array)[1])           ///< 按照LSB格式把两个字节转化为一个Word
#define HY_MEM_LSB_WORD_2_BYTE(_array, _word)   (_array)[0] = ((_word) / 256); (_array)[1] = ((_word) & 0xFF)   ///< 按照LSB格式把一个Word转化为两个字节

#define HY_MEM_ALIGN(_len, _align)              (((_len) + (_align) - 1) & ~((_align) - 1))                     ///< 字节对齐
#define HY_MEM_ALIGN2(_len)                     HY_MEM_ALIGN(_len, 2)                                           ///< 2字节对齐
#define HY_MEM_ALIGN4(_len)                     HY_MEM_ALIGN(_len, 4)                                           ///< 4字节对齐
#define HY_MEM_ALIGN4_UP(_len)                  (HY_MEM_ALIGN(_len, 4) + HY_MEM_ALIGN4(1))                      ///< 4字节向上对齐(原来已经事4字节对齐，使用后再增加4个字节)

#define HY_MEMCMP(_dst, _src, _len)             memcmp(_dst, _src, _len)
#define HY_MEMSET(_ptr, _len)                   memset(_ptr, '\0', _len)
#define HY_MEMCPY(_dst, _src, _len)             memcpy(_dst, _src, _len)

#define HY_MEM_CALLOC_BREAK(_type, _len)            \
({                                                  \
    void *ptr = calloc(1, _len);                    \
    if (!ptr) {                                     \
        LOGES("calloc failed \n");                  \
        break;                                      \
    }                                               \
    (_type)ptr;                                     \
})

#define HY_MEM_CALLOC_RETURN(_type, _len)           \
({                                                  \
    void *ptr = calloc(1, _len);                    \
    if (!ptr) {                                     \
        LOGES("calloc failed \n");                  \
        return;                                     \
    }                                               \
    (_type)ptr;                                     \
})

#define HY_MEM_CALLOC_RETURN_VAL(_type, _len, _val) \
({                                                  \
    void *ptr = calloc(1, _len);                    \
    if (!ptr) {                                     \
        LOGES("calloc failed \n");                  \
        return _val;                                \
    }                                               \
    (_type)ptr;                                     \
})

#define HY_MEM_MALLOC_BREAK(_type, _len)            \
({                                                  \
    void *ptr = malloc(_len);                       \
    if (!ptr) {                                     \
        LOGES("malloc failed \n");                  \
        break;                                      \
    } else {                                        \
        HY_MEMSET(ptr, _len);                       \
    }                                               \
    (_type)ptr;                                     \
})

#define HY_MEM_MALLOC_RET(_type, _len)              \
({                                                  \
    void *ptr = malloc(_len);                       \
    if (!ptr) {                                     \
        LOGES("malloc failed \n");                  \
        return;                                     \
    } else {                                        \
        HY_MEMSET(ptr, _len);                       \
    }                                               \
    (_type)ptr;                                     \
})

#define HY_MEM_MALLOC_RET_VAL(_type, _len, ret)     \
({                                                  \
    void *ptr = malloc(_len);                       \
    if (!ptr) {                                     \
        LOGES("malloc failed \n");                  \
        return ret;                                 \
    } else {                                        \
        HY_MEMSET(ptr, _len);                       \
    }                                               \
    (_type)ptr;                                     \
})

#define HY_MEM_FREE_P(_ptr)                         \
do {                                                \
    if (_ptr) {                                     \
        free(_ptr);                                 \
        (_ptr) = NULL;                              \
    }                                               \
} while (0)

#define HY_MEM_FREE_PP(_pptr)                       \
do {                                                \
    if (*_pptr) {                                   \
        free(*_pptr);                               \
        (*_pptr) = NULL;                            \
    }                                               \
} while (0)

/**
 * @brief 申请内存
 *
 * @param size 需要分配的长度
 *
 * @return 成功返回内存的地址，失败返回NULL
 */
void *HyMemMalloc(hy_s32_t size);

/**
 * @brief 释放内存
 *
 * @param pptr 需要释放内存的二级指针
 */
void HyMemFree(void **pptr);

/**
 * @brief 申请内存
 *
 * @param nmemb 申请内存的个数
 * @param size 每个内存对应的长度
 *
 * @return 成功返回内存的地址，失败返回NULL
 */
void *HyMemCalloc(hy_s32_t nmemb, hy_s32_t size);

/**
 * @brief 申请内存
 *
 * @param ptr 原来内存的地址
 * @param nmemb 申请内存的个数
 * @param size 每个内存对应的长度
 *
 * @return 成功返回内存的地址，失败返回NULL
 */
void *HyMemRealloc(void *ptr, hy_s32_t nmemb, hy_s32_t size);

#ifdef __cplusplus
}
#endif

#endif

