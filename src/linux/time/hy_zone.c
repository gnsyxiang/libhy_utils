/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_zone.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    13/04 2022 11:03
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        13/04 2022      create the file
 * 
 *     last modified: 13/04 2022 11:03
 */
#include <stdio.h>
#include <time.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_string.h"
#include "hy_mem.h"
#include "hy_utils.h"
#include "hy_linux.h"

#include "hy_zone.h"

// 链接文件在res/zoneinfo目录下

// 东时区为正，西时区为负

// CAT	东部非洲时间
// CEST	中欧夏令时间
// IST	以色列标准时间
// SAST	南部非洲时间
// USZ1	加里宁格勒时间
// WAST	西部非洲夏令时间
typedef struct {
    const char          *zoneinfo_path;      // 配置文件路径
    const char          *zoneinfo_name;      // 各地标准时间缩写，用于putenv("TZ=CST6CDT");设置环境变量
} _zone_s;

typedef struct {
    HyZoneType_e        type;
    HyZoneNum_e         num;
    const char          *GMT_path;

    _zone_s             *zone;
    hy_u32_t            zone_cnt;
} _time_zone_s;

typedef struct {
    HyZoneSaveConfig_s  save_c;
} _zone_context_s;

static _zone_context_s *context = NULL;

static _zone_s zone_east_0[] = {
    {"Europe/Lisbon",           "WET0WEST"},        // 葡萄牙.里斯本
};

static _zone_s zone_east_1[] = {
    {"Atlantic/Azores",         ""},                // 葡萄牙.亚速尔群岛
    {"Europe/Paris",            "CET-1CEST"},       // 法国.巴黎
};

static _zone_s zone_east_2[] = {
    {"Africa/Cairo",            "EET-2"},           // 埃及.开罗
    {"Africa/Johannesburg",     "SAST-2"},          // 南非.约翰内斯堡
    {"Europe/Kaliningrad",      "EET-2"},           // 俄罗斯.加里寧格勒
};

static _zone_s zone_east_3[] = {
    {"Europe/Moscow",           "MSK-3"},           // 俄罗斯.莫斯科
};

static _zone_s zone_east_4[] = {
    {"Europe/Samara",           ""},                // 俄罗斯.萨马拉
    {"Asia/Dubai",              ""},                // 阿拉伯.迪拜
};

static _zone_s zone_east_5[] = {
};

static _zone_s zone_east_6[] = {
};

static _zone_s zone_east_7[] = {
    {"Asia/Krasnoyarsk",        ""},                // 俄罗斯.克拉斯諾亞爾斯克
    {"Asia/Jakarta",            "WIB-7"},           // 印度尼西亚.雅加达
};

static _zone_s zone_east_8[] = {
    {"Asia/Shanghai",           "CST-8"},           // 中国.上海
    {"Asia/Urumqi",             "CST-8"},           // 中国.乌鲁木齐
    {"Asia/Hong_Kong",          "HKT-8"},           // 中国.香港
    {"Asia/Makassar",           "WITA-8"},          // 印度尼西亚.望加锡
};

static _zone_s zone_east_9[] = {
    {"Asia/Jayapura",           "WIT-9"},           // 印度尼西亚.查亚普拉
};

static _zone_s zone_east_10[] = {
};

static _zone_s zone_east_11[] = {
    {"Pacific/Midway",          ""},
};

static _zone_s zone_east_12[] = {
    {"Pacific/Kwajalein",       ""},
    {"Asia/Anadyr",             ""},                // 俄罗斯.阿纳德尔
};

//----

static _zone_s zone_west_0[] = {
};

static _zone_s zone_west_1[] = {
};

static _zone_s zone_west_2[] = {
};

static _zone_s zone_west_3[] = {
};

static _zone_s zone_west_4[] = {
};

static _zone_s zone_west_5[] = {
    {"America/New_York",        ""}
};

static _zone_s zone_west_6[] = {
    {"America/Chicago",         "CST6CDT"}          // 美国.芝加哥
};

static _zone_s zone_west_7[] = {
};

static _zone_s zone_west_8[] = {
};

static _zone_s zone_west_9[] = {
};

static _zone_s zone_west_10[] = {
};

static _zone_s zone_west_11[] = {
};

static _zone_s zone_west_12[] = {
};

#define _ZONE_INIT(_zone) _zone, HY_UTILS_ARRAY_CNT(_zone)

static _time_zone_s time_zone[] = {
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_0,  "Etc/GMT-0",  _ZONE_INIT(zone_east_0)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_1,  "Etc/GMT-1",  _ZONE_INIT(zone_east_1)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_2,  "Etc/GMT-2",  _ZONE_INIT(zone_east_2)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_3,  "Etc/GMT-3",  _ZONE_INIT(zone_east_3)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_4,  "Etc/GMT-4",  _ZONE_INIT(zone_east_4)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_5,  "Etc/GMT-5",  _ZONE_INIT(zone_east_5)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_6,  "Etc/GMT-6",  _ZONE_INIT(zone_east_6)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_7,  "Etc/GMT-7",  _ZONE_INIT(zone_east_7)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_8,  "Etc/GMT-8",  _ZONE_INIT(zone_east_8)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_9,  "Etc/GMT-9",  _ZONE_INIT(zone_east_9)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_10, "Etc/GMT-10", _ZONE_INIT(zone_east_10)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_11, "Etc/GMT-11", _ZONE_INIT(zone_east_11)},
    {HY_ZONE_TYPE_EAST, HY_ZONE_NUM_12, "Etc/GMT-12", _ZONE_INIT(zone_east_12)},

    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_0,  "Etc/GMT+0",  _ZONE_INIT(zone_west_0)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_1,  "Etc/GMT+1",  _ZONE_INIT(zone_west_1)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_2,  "Etc/GMT+2",  _ZONE_INIT(zone_west_2)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_3,  "Etc/GMT+3",  _ZONE_INIT(zone_west_3)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_4,  "Etc/GMT+4",  _ZONE_INIT(zone_west_4)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_5,  "Etc/GMT+5",  _ZONE_INIT(zone_west_5)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_6,  "Etc/GMT+6",  _ZONE_INIT(zone_west_6)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_7,  "Etc/GMT+7",  _ZONE_INIT(zone_west_7)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_8,  "Etc/GMT+8",  _ZONE_INIT(zone_west_8)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_9,  "Etc/GMT+9",  _ZONE_INIT(zone_west_9)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_10, "Etc/GMT+10", _ZONE_INIT(zone_west_10)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_11, "Etc/GMT+11", _ZONE_INIT(zone_west_11)},
    {HY_ZONE_TYPE_WEST, HY_ZONE_NUM_12, "Etc/GMT+12", _ZONE_INIT(zone_west_12)},
};

static hy_s32_t _zone_set(const char *zoneinfo_path)
{
    // 由于根文件系统只读，所以借助/tmp/localtime中转一下
    // 在制作根文件系统时，建立ln -s /tmp/localtime /etc/localtime
    // 最后把/tmp/localtime链接到设置的时区上

    hy_s32_t ret = -1;
    char cmd[256] = {0};

    LOGI("zone info path: %s \n", zoneinfo_path);

    HY_MEMSET(cmd, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "rm -rf /tmp/localtime");
    ret = HyUtilsSystemCmd_m(cmd, 0);
    if (0 != ret) {
        return -1;
    }

    HY_MEMSET(cmd, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "ln -s %s/%s /tmp/localtime",
            context->save_c.zone_file_paht, zoneinfo_path);
    ret = HyUtilsSystemCmd_m(cmd, 0);
    if (0 != ret) {
        return -1;
    }

    // 使用环境变量TZ的当前设置,
    // 把值赋给三个全局变量:daylight,timezone和tzname。
    // 这些变量由ftime和localtime函数使用校正格林威治(UTC)时间为本地时间,
    // 通过time函数从系统时间计算UTC
    tzset();
    LOGI("daylight: %d \n", daylight); 
    LOGI("timezone: %ld \n", timezone); 
    LOGI("tzname[0]: %s \n", tzname[0]);
    LOGI("tzname[1]: %s \n", tzname[1]);

    context->save_c.zone_info.daylight = daylight;
    context->save_c.zone_info.utc_s = timezone;

    /* @note: <22-04-14, uos> 这之后获取的时间就是更改时区之后的 */
    return 0;
}

static void _save_zone_info(_time_zone_s *time_zone, _zone_s *zone)
{
    HyZoneInfo_s *zone_info_tmp = &context->save_c.zone_info;

    HY_MEMSET(zone_info_tmp, sizeof(*zone_info_tmp));
    zone_info_tmp->type = time_zone->type;
    zone_info_tmp->num = time_zone->num;
    if (zone && HY_STRLEN(zone->zoneinfo_path) > 0) {
        HY_STRCPY(zone_info_tmp->zoneinfo_path, zone->zoneinfo_path);
    }
    if (zone && HY_STRLEN(zone->zoneinfo_name) > 0) {
        HY_STRCPY(zone_info_tmp->zoneinfo_name, zone->zoneinfo_name);
    }
}

hy_s32_t HyZoneSet(HyZoneInfo_s *zone_info)
{
    HY_ASSERT_RET_VAL(!zone_info, -1);

    hy_u32_t i = 0;
    hy_u32_t j = 0;
    _zone_s *zone = NULL;

    for (i = 0; i < HY_UTILS_ARRAY_CNT(time_zone); ++i) {
        zone = time_zone[i].zone;

        for (j = 0; j < time_zone[i].zone_cnt; ++j) {
            // 1, 优先配置文件
            if (HY_STRLEN(zone_info->zoneinfo_path) > 0
                    && 0 == HY_STRCMP(zone_info->zoneinfo_path,
                        zone[j].zoneinfo_path)) {
                _save_zone_info(&time_zone[i], &zone[j]);
                return  _zone_set(time_zone[i].zone[j].zoneinfo_path);
            }

            // 2, 地方标准时间
            if (HY_STRLEN(zone_info->zoneinfo_name) > 0
                    && 0 == HY_STRCMP(zone_info->zoneinfo_name,
                        zone[j].zoneinfo_name)) {
                _save_zone_info(&time_zone[i], &zone[j]);
                return  _zone_set(time_zone[i].zone[j].zoneinfo_path);
            }
        }
    }

    for (i = 0; i < HY_UTILS_ARRAY_CNT(time_zone); ++i) {
        // 3, 指定GMT时区
        if (time_zone[i].type == zone_info->type
                && time_zone[i].num == zone_info->num) {
            _save_zone_info(&time_zone[i], NULL);
            return  _zone_set(time_zone[i].GMT_path);
        }
    }

    LOGE("zone set failed \n");
    return -1;
}

hy_s32_t HyZoneGet(HyZoneInfo_s *zone_info)
{
    HY_ASSERT_RET_VAL(!zone_info, -1);

    HY_MEMCPY(zone_info, &context->save_c.zone_info, sizeof(*zone_info));

    return 0;
}

void HyZoneDestroy(void **handle)
{
    LOGI("zone destroy, context: %p \n", context);
    HY_MEM_FREE_PP(&context);
}

void *HyZoneCreate(HyZoneConfig_s *zone_c)
{
    HY_ASSERT_RET_VAL(!zone_c, NULL);

    do {
        context = HY_MEM_MALLOC_BREAK(_zone_context_s *, sizeof(*context));
        HY_MEMCPY(&context->save_c, &zone_c->save_c, sizeof(context->save_c));

        LOGI("zone create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("zone create failed \n");
    HyZoneDestroy((void **)&context);
    return NULL;
}

