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
#include <unistd.h>

#include "hy_assert.h"
#include "hy_string.h"
#include "hy_mem.h"
#include "hy_gpio.h"

#include "thread/hy_thread.h"

#include "hy_utils.h"

#include "net_wifi.h"

#define _WIFI_CONFIG_PATH "/tmp/wifi.conf"
#define _EXEC_CMD(_cmd)                     \
    do {                                    \
        LOGI("execute cmd: %s\n", _cmd);    \
        system(_cmd);                       \
    } while (0)

typedef struct {
    NetWifiSaveConfig_t     save_c;

    void                    *thread_h;
} _wifi_context_s;

static hy_s32_t _connect_wifi(_wifi_context_s *context,
        HyNetIpInfo_s *wifi_ip_info)
{
    HyNetWifiConfig_s *wifi_c = context->save_c.wifi_c;
    char cmd[512] = {0};
    hy_s32_t ret = -1;

    do {
        HY_MEMSET(cmd, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "ifconfig %s up", wifi_c->name);
        _EXEC_CMD(cmd);

        HY_MEMSET(cmd, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s", "killall wpa_supplicant");
        _EXEC_CMD(cmd);

        HY_MEMSET(cmd, sizeof(cmd));
        snprintf(cmd, sizeof(cmd),
                "wpa_passphrase \"%s\" \"%s\" > "_WIFI_CONFIG_PATH
                " && sed -i '2i \tscan_ssid=1' "_WIFI_CONFIG_PATH
#ifdef NDEBUG
                " && sed -i '/#psk=/d' "_WIFI_CONFIG_PATH
#endif
                " && wpa_supplicant -B -D%s -i%s -c "_WIFI_CONFIG_PATH,
                wifi_c->ssid, wifi_c->pwd, wifi_c->driver_name, wifi_c->name);
        _EXEC_CMD(cmd);

        HY_MEMSET(cmd, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "%s", "killall udhcpc");
        _EXEC_CMD(cmd);

        if (wifi_c->dhcp) {
            HY_MEMSET(cmd, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "udhcpc -i %s &", wifi_c->name);
            _EXEC_CMD(cmd);
        } else {
            char ip[HY_UTILS_IP_STR_LEN_MAX] = {0};
            char mask[HY_UTILS_IP_STR_LEN_MAX] = {0};
            char gw[HY_UTILS_IP_STR_LEN_MAX] = {0};
            char dns1[HY_UTILS_IP_STR_LEN_MAX] = {0};
            char dns2[HY_UTILS_IP_STR_LEN_MAX] = {0};

            HyUtilsIpInt2Str(wifi_ip_info->ip, ip, HY_UTILS_IP_STR_LEN_MAX);
            HyUtilsIpInt2Str(wifi_ip_info->mask, mask, HY_UTILS_IP_STR_LEN_MAX);
            HyUtilsIpInt2Str(wifi_ip_info->gw, gw, HY_UTILS_IP_STR_LEN_MAX);
            HyUtilsIpInt2Str(wifi_ip_info->dns1, dns1, HY_UTILS_IP_STR_LEN_MAX);
            HyUtilsIpInt2Str(wifi_ip_info->dns2, dns2, HY_UTILS_IP_STR_LEN_MAX);

            HY_MEMSET(cmd, sizeof(cmd));
            snprintf(cmd, sizeof(cmd),
                    "ifconfig %s %s netmask %s", wifi_c->name, ip, mask);
            _EXEC_CMD(cmd);

            HY_MEMSET(cmd, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "route add default gw %s", gw);
            _EXEC_CMD(cmd);

            HY_MEMSET(cmd, sizeof(cmd));
            snprintf(cmd, sizeof(cmd),
                    "echo 'nameserver %s\nnameserver %s' > /etc/resolv.conf",
                    dns1, dns2);
            _EXEC_CMD(cmd);
        }

        ret = 0;
    } while (0);

    return ret;
}

static hy_s32_t _thread_cb(void *args)
{
    _wifi_context_s *context = args;
    HyNetIpInfo_s *wifi_ip_info = NULL;

    HyGpioSetVal(&context->save_c.gpio, HY_GPIO_VAL_OFF);
    sleep(3);
    HyGpioSetVal(&context->save_c.gpio, HY_GPIO_VAL_ON);
    sleep(1);

    for (int i = 0; i < HY_NET_WIFI_CONFIG_CNT_MAX; ++i) {
        wifi_ip_info = &context->save_c.wifi_ip_info[i];
        if (0 == _connect_wifi(context, wifi_ip_info)) {
            break;
        }
    }

    return -1;
}

void net_wifi_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _wifi_context_s *context = *handle;
    HyNetWifiConfig_s *wifi_c = context->save_c.wifi_c;
    char cmd[128] = {0};

    HY_MEMSET(cmd, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "%s", "killall udhcpc wpa_supplicant");
    _EXEC_CMD(cmd);

    HY_MEMSET(cmd, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "ifconfig %s down", wifi_c->name);
    _EXEC_CMD(cmd);

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

