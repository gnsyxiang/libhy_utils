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
 * @brief 
 */
typedef enum {
    HY_ZONE_TYPE_EAST,  ///< 
    HY_ZONE_TYPE_WEST,  ///< 
} HyZoneType_e;

/**
 * @brief 
 */
typedef enum {
    HY_ZONE_NUM_0,      ///< 
    HY_ZONE_NUM_1,      ///< 
    HY_ZONE_NUM_2,      ///< 
    HY_ZONE_NUM_3,      ///< 
    HY_ZONE_NUM_4,      ///< 
    HY_ZONE_NUM_5,      ///< 
    HY_ZONE_NUM_6,      ///< 
    HY_ZONE_NUM_7,      ///< 
    HY_ZONE_NUM_8,      ///< 
    HY_ZONE_NUM_9,      ///< 
    HY_ZONE_NUM_10,     ///< 
    HY_ZONE_NUM_11,     ///< 
    HY_ZONE_NUM_12,     ///< 
} HyZoneNum_e;

/**
 * @brief 
 */
typedef struct {
    HyZoneType_e    type;                                       ///< 
    HyZoneNum_e     num;                                        ///< 
    char            zoneinfo_path[HY_ZONE_INFO_PATH_LEN_MAX];   ///< 
    char            zoneinfo_name[HY_ZONE_INFO_NAME_LEN_MAX];   ///< 
} HyZoneInfo_s;


hy_s32_t HyZoneSet(HyZoneInfo_s *zone_info);

hy_s32_t HyZoneGet(HyZoneInfo_s *zone_info);

#ifdef __cplusplus
}
#endif

#endif

