/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_time.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    20/12 2021 14:31
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        20/12 2021      create the file
 * 
 *     last modified: 20/12 2021 14:31
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_TIME_H_
#define __LIBHY_UTILS_INCLUDE_HY_TIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <sys/time.h>

#include <hy_os_type/hy_type.h>

/**
 * @brief 判断指针类型是否相同，类型不相同编译器会报错
 *
 * @param _ptr1 参数1，必须是指针
 * @param _ptr2 参数2，必须是指针
 */
#define HY_TIME_CHECK_TYPE(_ptr1, _ptr2)                                    \
do {                                                                        \
    typeof(_ptr1) __ptr1 = (_ptr1);                                         \
    typeof(_ptr2) __ptr2 = (_ptr2);                                         \
    (void) (&__ptr1 == &__ptr2);                                            \
} while (0)

/**
 * @brief 计算"_tv1 + _tv2"时间(struct timeval)之和
 *
 * @param _tv1 时间1
 * @param _tv2 时间2
 * @param _sum_tv_ptr 时间之和（指针）
 */
#define HY_TIME_TIMEVAL_ADD(_tv1, _tv2, _sum_tv_ptr)                        \
do {                                                                        \
    if (NULL == _sum_tv_ptr) {                                              \
        LOGE("the param is NULL \n");                                       \
        break;                                                              \
    }                                                                       \
                                                                            \
    HY_TIME_CHECK_TYPE(&_tv1, &_tv2);                                       \
    HY_TIME_CHECK_TYPE(&_tv1, _sum_tv_ptr);                                 \
                                                                            \
    (_sum_tv_ptr)->tv_sec = (_tv1).tv_sec + (_tv2).tv_sec;                  \
    (_sum_tv_ptr)->tv_usec = (_tv1).tv_usec + (_tv2).tv_usec;               \
    if ((_sum_tv_ptr)->tv_usec >= 1000000) {                                \
        (_sum_tv_ptr)->tv_sec++;                                            \
        (_sum_tv_ptr)->tv_usec -= 1 * 1000 * 1000;                          \
    }                                                                       \
} while (0)

/**
 * @brief 计算"_tv1 - _tv2"时间(struct timeval)之差
 *
 * @param _tv1 时间1
 * @param _tv2 时间2
 * @param _sum_tv_ptr 时间之差（指针）
 */
#define HY_TIME_TIMEVAL_SUB(_tv1, _tv2, _sub_tv_ptr)                        \
do {                                                                        \
    if (NULL == _sub_tv_ptr) {                                              \
        LOGE("the param is NULL \n");                                       \
        break;                                                              \
    }                                                                       \
                                                                            \
    HY_TIME_CHECK_TYPE(&_tv1, &_tv2);                                       \
    HY_TIME_CHECK_TYPE(&_tv1, _sub_tv_ptr);                                 \
                                                                            \
    (_sub_tv_ptr)->tv_sec = (_tv1).tv_sec - (_tv2).tv_sec;                  \
    (_sub_tv_ptr)->tv_usec = (_tv1).tv_usec - (_tv2).tv_usec;               \
    if ((_sub_tv_ptr)->tv_usec < 0) {                                       \
        (_sub_tv_ptr)->tv_sec--;                                            \
        (_sub_tv_ptr)->tv_usec += 1 * 1000 * 1000;                          \
    }                                                                       \
} while (0)

/**
 * @brief 计算(当前时间 - _tv时间)(struct timeval)之差
 *
 * @param _tv 该时间为过去的时间
 * @param _now_sub_tv_ptr 时间之差（指针）
 */
#define HY_TIME_TIMEVAL_CUR_SUB(_tv, _now_sub_tv_ptr)                       \
do {                                                                        \
    struct timeval _cur_time;                                               \
                                                                            \
    if (NULL == _now_sub_tv_ptr) {                                          \
        LOGE("the param is NULL \n");                                       \
        break;                                                              \
    }                                                                       \
                                                                            \
    HY_TIME_CHECK_TYPE(&_tv, _now_sub_tv_ptr);                              \
                                                                            \
    gettimeofday(&_cur_time, NULL);                                         \
    HY_TIME_TIMEVAL_SUB(_cur_time, (_tv), (_now_sub_tv_ptr));               \
} while (0)

/**
 * @brief 计算(当前时间 - _tv时间)(struct timeval)之差，返回相差ms值
 *
 * @param _tv 该时间为过去的时间
 * @param _sub_ms_ptr 返回与当前时间的差值
 */
#define HY_TIME_TIMEVAL_CUR_SUB_MS(_tv, _sub_ms_ptr)                        \
do {                                                                        \
    struct timeval _difference;                                             \
    struct timeval _cur_time;                                               \
                                                                            \
    if (NULL == _sub_ms_ptr) {                                              \
        LOGE("the param is NULL \n");                                       \
        break;                                                              \
    }                                                                       \
                                                                            \
    HY_TIME_CHECK_TYPE(&_cur_time, &_tv);                                   \
                                                                            \
    gettimeofday(&_cur_time, NULL);                                         \
                                                                            \
    HY_TIME_TIMEVAL_SUB(_cur_time, _tv, &_difference);                      \
                                                                            \
    *_sub_ms_ptr = _difference.tv_sec * 1000 + _difference.tv_usec / 1000;  \
} while(0);

/**
 * @brief 计算(当前时间 - _tv2_ptr时间)(struct timeval)之差，返回相差ms值，同时更新_tv_ptr
 *
 * @param _tv_ptr 该时间为过去的时间（地址）
 * @param _sub_ms_ptr 返回与当前时间的差值
 */
#define HY_TIME_TIMEVAL_UPDATE_CUR_SUB_MS(_tv_ptr, _sub_ms_ptr)             \
do {                                                                        \
    struct timeval _difference;                                             \
    struct timeval _cur_time;                                               \
                                                                            \
    if (NULL == _sub_ms_ptr) {                                              \
        LOGE("the param is NULL \n");                                       \
        break;                                                              \
    }                                                                       \
                                                                            \
    HY_TIME_CHECK_TYPE(&_cur_time, _tv_ptr);                                \
                                                                            \
    gettimeofday(&_cur_time, NULL);                                         \
                                                                            \
    HY_TIME_TIMEVAL_SUB(_cur_time, (*_tv_ptr), &_difference);               \
                                                                            \
    *(_tv_ptr) = _cur_time;                                                 \
    *_sub_ms_ptr = _difference.tv_sec * 1000 + _difference.tv_usec / 1000;  \
} while(0);

/**
 * @brief 获取utc时间
 *
 * @return 微妙
 */
hy_u64_t HyTimeGetUTC_us(void);

/**
 * @brief 获取utc时间
 *
 * @return 毫秒
 */
hy_u64_t HyTimeGetUTC_ms(void);

/**
 * @brief 获取utc时间
 *
 * @return 秒
 */
time_t HyTimeGetUTC_s(void);

/**
 * @brief 转换时间
 *
 * @param ms ms
 *
 * @return 转换struct timespec
 */
struct timespec HyTimeGetTimespec(hy_u32_t ms);

/**
 * @brief 获取当前时间
 *
 * @param tm 当前时间
 *
 * @return 成功返回当前时间，失败回复NULL
 *
 * @note 返回值就是返回传入的指针，这样设计方便不同场景下的应用
 */
struct tm *HyTimeGetLocalTime(struct tm *tm);

/**
 * @brief 获取并格式化当前时间
 *
 * @param buf 数据
 * @param len 数据长度
 *
 * @return 返回buf中实际的长度
 */
hy_u32_t HyTimeFormatLocalTime(char *buf, hy_u32_t len);

/**
 * @brief 获取并格式化当前时间（提供到毫秒）
 *
 * @param buf 数据
 * @param len 数据长度
 *
 * @return 返回buf中的实际长度
 */
hy_u32_t HyTimeFormatLocalTime_ms(char *buf, hy_u32_t len);

/**
 * @brief 把格式化时间转成UTC时间
 *
 * @param data_time 被格式化的时间
 *
 * @return UTC时间
 *
 * @note 时间格式为: 2021-12-20 19:00:00
 */
time_t HyTimeFormatTime2UTC(const char *data_time);

/**
 * @brief 获取当天的零点和24点的UTC值
 *
 * @param start 零点值
 * @param end 24点值
 */
void HyTimeGetCurDayRegion(const time_t cur_utc, time_t *start, time_t *end);

/**
 * @brief 延时us
 *
 * @param us 延时时间
 */
void HyTimeDelay_us(hy_u32_t us);

/**
 * @brief 延时ms
 *
 * @param ms 延时时间
 */
void HyTimeDelay_ms(hy_u32_t ms);

/**
 * @brief 延时s
 *
 * @param s 延时时间
 */
void HyTimeDelay_s(hy_u32_t s);

#ifdef __cplusplus
}
#endif

#endif

