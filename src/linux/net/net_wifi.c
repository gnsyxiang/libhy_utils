/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    net_wifi.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    01/04 2022 16:36
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        01/04 2022      create the file
 * 
 *     last modified: 01/04 2022 16:36
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_barrier.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_log.h"

#include "net_wifi_config.h"
#include "net_wifi.h"

#define _WIFI_CONFIG_PATH   "./wifi_config.json"

typedef struct {
    NetWifiSaveConfig_t     save_c;

    net_wifi_config_s       net_wifi_config;
    void                    *net_wifi_config_h;
} _net_wifi_s;

void NetWifiDestroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _net_wifi_s *context = *handle;

    net_wifi_config_destroy(&context->net_wifi_config_h);

    LOGI("net wifi create, context: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *NetWifiCreate(NetWifiConfig_t *net_wifi_c)
{
    LOGT("net_wifi_c: %p \n", net_wifi_c);
    HY_ASSERT_RET_VAL(!net_wifi_c, NULL);

    if (!net_wifi_c->set_default_cb) {
        LOGE("please provide the default WiFi configuration \n");
        return NULL;
    }

    _net_wifi_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_net_wifi_s *, sizeof(*context));

        context->net_wifi_config_h = net_wifi_config_create_m(_WIFI_CONFIG_PATH,
                net_wifi_c->set_default_cb, net_wifi_c->args);
        if (!context->net_wifi_config_h) {
            LOGE("net_wifi_config_create_m failed \n");
            break;
        }

        LOGI("net wifi create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("net wifi create failed \n");
    NetWifiDestroy((void **)&context);
    return NULL;
}

