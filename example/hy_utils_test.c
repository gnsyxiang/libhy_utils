/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_utils_test.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 14:16
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 14:16
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hy_utils.h"

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_hal_utils.h"

#include "hy_log.h"

typedef struct {
    void *log_handle;
    void *signal_handle;

    hy_s32_t exit_flag;
} _main_context_t;

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_context_t *context = args;
    context->exit_flag = 1;
}

static void _signal_user_cb(void *args)
{
    LOGI("------user cb\n");

    _main_context_t *context = args;
    context->exit_flag = 1;
}

static void _module_destroy(_main_context_t **context_pp)
{
    _main_context_t *context = *context_pp;

    // note: 增加或删除要同步到module_create_t中
    module_destroy_t module[] = {
        {"signal",  &context->signal_handle,    HySignalDestroy},
        {"log",     &context->log_handle,       HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_t log_config;
    log_config.save_config.buf_len      = 512;
    log_config.save_config.level        = HY_LOG_LEVEL_TRACE;
    log_config.save_config.color_enable = HY_TYPE_FLAG_ENABLE;

    int8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    int8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGINT, SIGTERM, SIGUSR1, SIGUSR2,
    };

    HySignalConfig_t signal_config;
    memset(&signal_config, 0, sizeof(signal_config));
    HY_MEMCPY(signal_config.error_num, signal_error_num, sizeof(signal_error_num));
    HY_MEMCPY(signal_config.user_num, signal_user_num, sizeof(signal_user_num));
    signal_config.save_config.app_name      = "template";
    signal_config.save_config.coredump_path = "./";
    signal_config.save_config.error_cb      = _signal_error_cb;
    signal_config.save_config.user_cb       = _signal_user_cb;
    signal_config.save_config.args          = context;

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",     &context->log_handle,       &log_config,        (create_t)HyLogCreate,      HyLogDestroy},
        {"signal",  &context->signal_handle,    &signal_config,     (create_t)HySignalCreate,   HySignalDestroy},
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

    {
        uint32_t ip_num = 0;
        char *ip_str = "192.168.1.110";

        HyUtilsIpStr2Int(ip_str, &ip_num);
        LOGE("ip_num: %u \n", ip_num);
    }

    {
        uint32_t ip_num = 3232235886;
        char ip_str[IP_STR_DOT_LEN] = {0};

        HyUtilsIpInt2Str(ip_num, ip_str, sizeof(ip_str));
        LOGE("ip_str: %s \n", ip_str);
    }

    {
        char addr[] = {0x10, 0x11, 0x12};
        char buf[BUF_LEN] = {0};

        HyUtilsHex2Int2Str(addr, HyHalUtilsArrayCnt(addr), buf, sizeof(buf));
        LOGE("buf: %s \n", buf);
    }

    {
        char *buf = "016017018";
        char addr[3];

        HyUtilsStr2Int2Hex(buf, strlen(buf), addr, 3);
        for (int i = 0; i < 3; ++i) {
            LOGE("%02x \n", addr[i]);
        }
    }

    {
        uint32_t dec = 0;
        char *buf = "10101010";
        dec = HyUtilsBitStr2Dec(buf, strlen(buf));
        LOGE("dec: %d, %02x \n", dec, dec);
    }

    {
        uint32_t dec = 0xaa;
        char buf[BUF_LEN] = {0};

        HyUtilsDec2BitStr(dec, 8, buf, sizeof(buf));
        LOGE("buf: %s \n", buf);
    }

    while (!context->exit_flag) {
        sleep(1);
    }

    _module_destroy(&context);

    return 0;
}

