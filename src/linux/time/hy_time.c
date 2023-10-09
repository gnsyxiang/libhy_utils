/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_time.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    20/12 2021 19:16
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        20/12 2021      create the file
 * 
 *     last modified: 20/12 2021 19:16
 */
#include <stdio.h>
#include <errno.h>

#include "hy_mem.h"
#include "hy_string.h"
#include "hy_assert.h"

#include "hy_time.h"

/*
 * 1. struct timeval {
 *        long tv_sec;    //秒
 *        long tv_usec;   //微妙
 *    }
 * 2. struct timespec {
 *        long int tv_sec;  //秒
 *        long int tv_nsec; //纳秒
 *    }
 * 3. struct tm {
 *        int tm_set;       //秒钟 - 取值区间为[0,59]
 *        int tm_min;       //分钟 - 取值区间为[0,59]
 *        int tm_hour;      //时钟 - 取值区间为[0,23]
 *        int tm_mday;      //日期 - 取值区间为[1,31]
 *        int tm_mon;       //月份 - 取值区间为[0,11]
 *        int tm_year;      //年份 - 其值从1900开始
 *        int tm_wday;      //星期 - 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推
 *        int tm_yday;      //天数 - 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推
 *        int tm_isdst;     //夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。
 *    }
 */

hy_u64_t HyTimeGetUTC_us(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (hy_u64_t)(tv.tv_sec * 1000 * 1000 + tv.tv_usec);
}

hy_u64_t HyTimeGetUTC_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (hy_u64_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

time_t HyTimeGetUTC_s(void)
{
    return time(NULL);
}

struct timespec HyTimeGetTimespec(hy_u32_t ms)
{
    struct timespec ts;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000;

    return ts;
}

struct tm *HyTimeGetLocalTime(struct tm *tm)
{
    time_t t;

    if (!tm) {
        LOGE("the tm is NULL \n");
        return NULL;
    }

    HY_MEMSET(tm, sizeof(*tm));

    t = time(NULL);
    localtime_r(&t, tm);    // localtime_r可重入，localtime不可重入

    return tm;
}

hy_u32_t HyTimeFormatLocalTime(char *buf, hy_u32_t len)
{
    time_t t;
    struct tm tm;

    HY_MEMSET(buf, len);

    t = time(NULL);
    localtime_r(&t, &tm);    // localtime_r可重入，localtime不可重入

    // 2021-12-20_19-00-00，时分秒不用分号的原因是在fat32文件系统中无法识别

    // 跟上面的效果一样，strftime可以生成许多的格式
    // strftime(buf, len, "%Y-%m-%d_%H-%M-%S", &tm);

    return snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec);
}

hy_u32_t HyTimeFormatLocalTime_ms(char *buf, hy_u32_t len)
{
    time_t t = 0;
    struct tm tm;
    struct timeval tv;

    HY_MEMSET(buf, len);

    t = time(NULL);
    localtime_r(&t, &tm);

    gettimeofday(&tv, NULL);

    return snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec, (hy_u32_t)tv.tv_usec / 1000);
}

time_t HyTimeFormatTime2UTC(const char *data_time)
{
    struct tm tm;

    // 2021-12-20_19-00-00，时分秒不用分号的原因是在fat32文件系统中无法识别

    sscanf(data_time, "%04d-%02d-%02d %02d:%02d:%02d",
           &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
           &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    tm.tm_year  -= 1900;
    tm.tm_mon   -= 1;

    return mktime(&tm);
}

void HyTimeGetCurDayRegion(const time_t cur_utc, time_t *start, time_t *end)
{
    struct tm tm;
    localtime_r(&cur_utc, &tm);
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;

    *start = mktime(&tm);

    *end = 0;
    *end += *start + 86400 - 1;
}

static void _delay_com(hy_u32_t s, hy_u32_t us)
{
    struct timeval tv;
    int err;

    tv.tv_sec   = s;
    tv.tv_usec  = us;

    do {
        err = select(0, NULL, NULL, NULL, &tv);
    } while(err < 0 && errno == EINTR);
}

void HyTimeDelay_us(hy_u32_t us)
{
    _delay_com(0, us);
}

void HyTimeDelay_ms(hy_u32_t ms)
{
    _delay_com(0, ms * 1000);
}

void HyTimeDelay_s(hy_u32_t s)
{
    _delay_com(s, 0);
}
