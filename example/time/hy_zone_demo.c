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

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_type.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_signal.h"
#include "hy_module.h"
#include "hy_utils.h"
#include "hy_time.h"

#include "hy_zone.h"

#define _APP_NAME "hy_zone_demo"

typedef struct {
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

static void _bool_module_destroy(void)
{
    HyModuleDestroyBool_s bool_module[] = {
        {"signal",          HySignalDestroy },
        {"log",             HyLogDeInit     },
    };

    HY_MODULE_RUN_DESTROY_BOOL(bool_module);
}

static hy_s32_t _bool_module_create(_main_context_t *context)
{
    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(log_c));
    log_c.config_file               = "../res/hy_log/zlog.conf";
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.level              = HY_LOG_LEVEL_TRACE;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL;

    int8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    int8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGINT, SIGTERM, SIGUSR1, SIGUSR2,
    };

    HySignalConfig_t signal_c;
    memset(&signal_c, 0, sizeof(signal_c));
    HY_MEMCPY(signal_c.error_num, signal_error_num, sizeof(signal_error_num));
    HY_MEMCPY(signal_c.user_num, signal_user_num, sizeof(signal_user_num));
    signal_c.save_c.app_name        = _APP_NAME;
    signal_c.save_c.coredump_path   = "./";
    signal_c.save_c.error_cb        = _signal_error_cb;
    signal_c.save_c.user_cb         = _signal_user_cb;
    signal_c.save_c.args            = context;

    HyModuleCreateBool_s bool_module[] = {
        {"log",         &log_c,         (HyModuleCreateBoolCb_t)HyLogInit,          HyLogDeInit},
        {"signal",      &signal_c,      (HyModuleCreateBoolCb_t)HySignalCreate,     HySignalDestroy},
    };

    HY_MODULE_RUN_CREATE_BOOL(bool_module);
}

static void _handle_module_destroy(_main_context_t *context)
{
    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"zone",        &context->zone_h,       HyZoneDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_t *context)
{
    HyZoneConfig_s zone_c;
    HY_MEMSET(&zone_c, sizeof(zone_c));
    HY_STRCPY(zone_c.save_c.zone_file_paht, "/data/nfs/bin/zoneinfo");

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"zone",        &context->zone_h,       &zone_c,        (HyModuleCreateHandleCb_t)HyZoneCreate,         HyZoneDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

int main(int argc, char *argv[])
{
    _main_context_t *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_t *, sizeof(*context));

        if (0 != _bool_module_create(context)) {
            printf("_bool_module_create failed \n");
            break;
        }

        if (0 != _handle_module_create(context)) {
            LOGE("_handle_module_create failed \n");
            break;
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

    } while (0);

    _handle_module_destroy(context);
    _bool_module_destroy();
    HY_MEM_FREE_PP(&context);

    return 0;
}

