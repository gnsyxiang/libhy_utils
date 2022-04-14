/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_zone.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    12/04 2022 20:03
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        12/04 2022      create the file
 * 
 *     last modified: 12/04 2022 20:03
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_ZONE_H_
#define __LIBHY_UTILS_INCLUDE_HY_ZONE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

#define HY_ZONE_INFO_PATH_LEN_MAX   (64)
#define HY_ZONE_INFO_NAME_LEN_MAX   (32)

/**
 * @brief 时区
 */
typedef enum {
    HY_ZONE_TYPE_EAST,  ///< 东时区
    HY_ZONE_TYPE_WEST,  ///< 西时区
} HyZoneType_e;

/**
 * @brief 时区号
 */
typedef enum {
    HY_ZONE_NUM_0,
    HY_ZONE_NUM_1,
    HY_ZONE_NUM_2,
    HY_ZONE_NUM_3,
    HY_ZONE_NUM_4,
    HY_ZONE_NUM_5,
    HY_ZONE_NUM_6,
    HY_ZONE_NUM_7,
    HY_ZONE_NUM_8,
    HY_ZONE_NUM_9,
    HY_ZONE_NUM_10,
    HY_ZONE_NUM_11,
    HY_ZONE_NUM_12,
} HyZoneNum_e;

/**
 * @brief 时区结构体
 *
 * @note
 * 1，优先使用zoneinfo_path，这个时直接的链接文件
 * 2，其次使用zoneinfo_name，这个是各地标准时间的缩写，可以直接用putenv("TZ=CST-8")执行
 * 3，最后使用指定时区及其夏令时
 */
typedef struct {
    HyZoneType_e    type;                                       ///< 时区
    HyZoneNum_e     num;                                        ///< 时区号
    hy_s32_t        daylight;                                   ///< 夏令时
    char            zoneinfo_path[HY_ZONE_INFO_PATH_LEN_MAX];   ///< 链接文件
    char            zoneinfo_name[HY_ZONE_INFO_NAME_LEN_MAX];   ///< 各地标准时间
} HyZoneInfo_s;

/**
 * @brief 设置时区
 *
 * @param zone_info 时区结构体
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyZoneSet(HyZoneInfo_s *zone_info);

/**
 * @brief 获取时区
 *
 * @param zone_info 时区结构体
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyZoneGet(HyZoneInfo_s *zone_info);

#ifdef __cplusplus
}
#endif

#endif

