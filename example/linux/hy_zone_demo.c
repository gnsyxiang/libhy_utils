/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_zone_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    13/04 2022 18:25
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        13/04 2022      create the file
 * 
 *     last modified: 13/04 2022 18:25
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_time.h"

#include "hy_zone.h"

#define _APP_NAME "hy_zone_demo"

typedef struct {
    void        *log_h;
    void        *signal_h;

    void        *zone_h;

    hy_s32_t    exit_flag;
} _main_context_t;

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_context_t *context = args;
    context->exit_flag = 1;
}

static void _signal_user_cb(void *args)
{
    LOGW("------user cb\n");

    _main_context_t *context = args;
    context->exit_flag = 1;
}

static void _module_destroy(_main_context_t **context_pp)
{
    _main_context_t *context = *context_pp;

    // note: 增加或删除要同步到module_create_t中
    module_destroy_t module[] = {
        {"zone",        &context->zone_h,       HyZoneDestroy},
        {"signal",      &context->signal_h,     HySignalDestroy},
        {"log",         &context->log_h,        HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_s log_c;
    log_c.save_c.buf_len_min  = 512;
    log_c.save_c.buf_len_max  = 512;
    log_c.save_c.level        = HY_LOG_LEVEL_TRACE;
    log_c.save_c.color_enable = HY_TYPE_FLAG_ENABLE;

    int8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    int8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGINT, SIGTERM, SIGUSR1, SIGUSR2,
    };

    HySignalConfig_t signal_c;
    HY_MEMSET(&signal_c, sizeof(signal_c));
    HY_MEMCPY(signal_c.error_num, signal_error_num, sizeof(signal_error_num));
    HY_MEMCPY(signal_c.user_num, signal_user_num, sizeof(signal_user_num));
    signal_c.save_c.app_name      = _APP_NAME;
    signal_c.save_c.coredump_path = "./";
    signal_c.save_c.error_cb      = _signal_error_cb;
    signal_c.save_c.user_cb       = _signal_user_cb;
    signal_c.save_c.args          = context;

    HyZoneConfig_s zone_c;
    HY_MEMSET(&zone_c, sizeof(zone_c));
    HY_STRCPY(zone_c.save_c.zone_file_paht, "/data/nfs/bin/zoneinfo");

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",         &context->log_h,        &log_c,         (create_t)HyLogCreate,          HyLogDestroy},
        {"signal",      &context->signal_h,     &signal_c,      (create_t)HySignalCreate,       HySignalDestroy},
        {"zone",        &context->zone_h,       &zone_c,        (create_t)HyZoneCreate,         HyZoneDestroy},
    };

    RUN_CREATE(module);

    return context;
}

int main(int argc, char *argv[])
{
    _main_context_t *context = _module_create();
    if (!context) {
        LOGE("_module_create faild \n");
        return -1;
    }

    LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    HyZoneInfo_s zone_info;
    HY_MEMSET(&zone_info, sizeof(zone_info));
#if 1
    #define _ZONEINFO_PATH "Asia/Shanghai"
    // #define _ZONEINFO_PATH "America/Chicago"
    HY_STRNCPY(zone_info.zoneinfo_path, sizeof(zone_info.zoneinfo_path),
            _ZONEINFO_PATH, HY_STRLEN(_ZONEINFO_PATH));
#endif

#if 0
    #define _ZONEINFO_NAME "CST-8"
    // #define _ZONEINFO_NAME "CST6CDT"
    HY_STRNCPY(zone_info.zoneinfo_name, sizeof(zone_info.zoneinfo_name),
            _ZONEINFO_NAME, sizeof(_ZONEINFO_NAME));
#endif

#if 0
    zone_info.type = HY_ZONE_TYPE_EAST;
    zone_info.num = HY_ZONE_NUM_8;
    // zone_info.type = HY_ZONE_TYPE_WEST;
    // zone_info.num = HY_ZONE_NUM_6;
#endif

    if (0 != HyZoneSet(&zone_info)) {
        LOGE("HyZoneSet failed \n");
    }

    HyZoneInfo_s zone_info_get;
    HyZoneGet(&zone_info_get);
    LOGI("type: %d \n", zone_info_get.type);
    LOGI("num: %d \n", zone_info_get.num);
    LOGI("daylight: %d \n", zone_info_get.daylight);
    LOGI("utc_s: %d \n", zone_info_get.utc_s);
    LOGI("zoneinfo_path: %s \n", zone_info_get.zoneinfo_path);
    LOGI("zoneinfo_name: %s \n", zone_info_get.zoneinfo_name);

    char time_buf[BUF_LEN] = {0};
    HyTimeFormatLocalTime(time_buf, sizeof(time_buf));
    LOGI("time_buf: %s \n", time_buf);

    while (!context->exit_flag) {
        sleep(1);
    }

    _module_destroy(&context);

    return 0;
}

