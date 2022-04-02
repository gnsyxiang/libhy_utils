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
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_gpio.h"

#include "net_wifi.h"

typedef struct {
    NetWifiSaveConfig_t     save_c;

    void                    *thread_h;
} _wifi_context_s;

static hy_s32_t _thread_cb(void *args)
{
    _wifi_context_s *context = args;
    HyNetWifiConfig_s *wifi_c = context->save_c.wifi_c;
    char cmd[256] = {0};

    HyGpioSetVal(&context->save_c.gpio, HY_GPIO_VAL_OFF);
    sleep(3);
    HyGpioSetVal(&context->save_c.gpio, HY_GPIO_VAL_ON);
    sleep(1);

    do {
        memset(cmd, '\0', sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "ifconfig %s up", wifi_c->name);
        LOGI("execute cmd: %s\n", cmd);
        system(cmd);

        memset(cmd, '\0', sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s", "killall wpa_supplicant");
        LOGI("execute cmd: %s\n", cmd);
        system(cmd);

        memset(cmd, '\0', sizeof(cmd));
        snprintf(cmd, sizeof(cmd),
                "wpa_passphrase \"%s\" \"%s\" > /tmp/wifi.conf;"
                "sed -i '2i scan_ssid=1' /tmp/wifi.conf;"
                "wpa_supplicant -Dwext -i%s -c /tmp/wifi.conf &",
                wifi_c->ssid, wifi_c->pwd, wifi_c->name);
        LOGI("execute cmd: %s\n", cmd);
        system(cmd);

        memset(cmd, '\0', sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s", "killall udhcpc");
        LOGI("execute cmd: %s\n", cmd);
        system(cmd);

        if (wifi_c->dhcp) {
            memset(cmd, '\0', sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "udhcpc -i %s &", wifi_c->name);
            LOGI("execute cmd: %s\n", cmd);
            system(cmd);
            break;
        } else {
        }
    } while (0);

    return -1;
}

void net_wifi_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _wifi_context_s *context = *handle;

    LOGI("net wifi create, context: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *net_wifi_create(NetWifiConfig_t *net_wifi_c)
{
    LOGT("net_wifi_c: %p \n", net_wifi_c);
    HY_ASSERT_RET_VAL(!net_wifi_c, NULL);

    _wifi_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_wifi_context_s *, sizeof(*context));
        HY_MEMCPY(&context->save_c, &net_wifi_c->save_c, sizeof(context->save_c));

        context->thread_h = HyThreadCreate_m("HYNW_wifi_reset",
                _thread_cb, context);
        if (!context->thread_h) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        LOGI("net wifi create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("net wifi create failed \n");
    net_wifi_destroy((void **)&context);
    return NULL;
}

