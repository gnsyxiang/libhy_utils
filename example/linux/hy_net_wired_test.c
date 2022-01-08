/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_net_wired_test.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 11:14
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 11:14
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hy_hal/hy_module.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_type.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#include "hy_net_wired.h"

typedef struct {
    void *log_handle;
    void *net_wired_handle;
} _main_context_t;

static void _module_destroy(_main_context_t **context_pp)
{
    _main_context_t *context = *context_pp;

    // note: 增加或删除要同步到module_create_t中
    module_destroy_t module[] = {
        {"net",     &context->net_wired_handle, HyNetWiredDestroy},
        {"log",     &context->log_handle,       HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_t log_config;
    log_config.save_config.buf_len_min  = 512;
    log_config.save_config.buf_len_max  = 512;
    log_config.save_config.level        = HY_LOG_LEVEL_TRACE;
    log_config.save_config.color_enable = HY_TYPE_FLAG_ENABLE;

    HyNetWiredLed_t led[HY_NET_WIRED_LED_MAX][HY_NET_WIRED_LED_MODE_MAX] = {
        {
            {HY_NET_WIRED_LED_MODE_OFF,         {{0x1e, 0x40c0}, {0x1f, 0x8000}}},
            {HY_NET_WIRED_LED_MODE_ON,          {{0x1e, 0x40c0}, {0x1f, 0xa000}}},
            {HY_NET_WIRED_LED_MODE_SLOW_BLINK,  {{0x1e, 0x40c0}, {0x1f, 0xc000}}},
            {HY_NET_WIRED_LED_MODE_FAST_BLINK,  {{0x1e, 0x40c0}, {0x1f, 0xe000}}},
        },
        {
            {HY_NET_WIRED_LED_MODE_OFF,         {{0x1e, 0x40c3}, {0x1f, 0x8000}}},
            {HY_NET_WIRED_LED_MODE_ON,          {{0x1e, 0x40c3}, {0x1f, 0xa000}}},
            {HY_NET_WIRED_LED_MODE_SLOW_BLINK,  {{0x1e, 0x40c3}, {0x1f, 0xc000}}},
            {HY_NET_WIRED_LED_MODE_FAST_BLINK,  {{0x1e, 0x40c3}, {0x1f, 0xe000}}},
        },
    };

    HyNetWiredConfig_t net_wired_config;
    #define _DEV_NAME "eth0"
    HY_STRNCPY(net_wired_config.save_config.dev_name, HY_NET_WIRED_DEV_NAME_LEN_MAX, _DEV_NAME, HY_STRLEN(_DEV_NAME));
    HY_MEMCPY(net_wired_config.save_config.led, &led, sizeof(led));

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",  &context->log_handle,          &log_config,        (create_t)HyLogCreate,      HyLogDestroy},
        {"net",  &context->net_wired_handle,    &net_wired_config,  (create_t)HyNetWiredCreate, HyNetWiredDestroy},
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

    HyNetWiredSetLed(HY_NET_WIRED_LED_0, HY_NET_WIRED_LED_MODE_SLOW_BLINK);
    HyNetWiredSetLed(HY_NET_WIRED_LED_1, HY_NET_WIRED_LED_MODE_FAST_BLINK);

    while (1) {
        // HyNetWiredSetLed(HY_NET_WIRED_LED_0, HY_NET_WIRED_LED_MODE_ON);
        // HyNetWiredSetLed(HY_NET_WIRED_LED_1, HY_NET_WIRED_LED_MODE_OFF);
        // usleep(100 * 1000);
        //
        // HyNetWiredSetLed(HY_NET_WIRED_LED_0, HY_NET_WIRED_LED_MODE_OFF);
        // HyNetWiredSetLed(HY_NET_WIRED_LED_1, HY_NET_WIRED_LED_MODE_ON);
        usleep(100 * 1000);
    }

    _module_destroy(&context);

    return 0;
}

