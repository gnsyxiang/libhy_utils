/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_net.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    02/04 2022 10:30
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        02/04 2022      create the file
 * 
 *     last modified: 02/04 2022 10:30
 */
#include <stdio.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_string.h"
#include "hy_mem.h"
#include "hy_file.h"
#include "thread/hy_thread_mutex.h"

#include "net_config.h"
#include "net_wifi.h"
#include "hy_net.h"

typedef struct {
    HyNetSaveConfig_s   save_c;

    void                *config_path_mutex_h;

    void                *eth_h;
    HyNetEthConfig_s    eth_c;
    HyNetIpInfo_s       eth_ip_info;

    void                *wifi_h;
    HyNetWifiConfig_s   wifi_c;
    HyNetIpInfo_s       wifi_ip_info[HY_NET_WIFI_CONFIG_CNT_MAX];
} _net_context_s;

static hy_s32_t _get_cmd_ret(const char *cmd_buf, hy_s32_t ret)
{
    HY_ASSERT(cmd_buf);

    FILE *fp = popen(cmd_buf, "r");
    if (!fp) {
        LOGES("popen failed \n");
        return -1;
    }

    char buf[8] = {0};
    fread(buf, sizeof(char), sizeof(buf) - 1, fp);

    pclose(fp);

    return (atoi(buf) == ret) ? 0 : -1;
}

hy_s32_t HyNetIsConnect(const char *dst)
{
    HY_ASSERT(dst);

    char cmd_buf[256] = {0};
    #define _PING_FORMAT "ping %s -c1 -w2 %s | grep time= | wc -l"

    for (int i = 0; i < 2; ++i) {
        HY_MEMSET(cmd_buf, sizeof(cmd_buf));
        snprintf(cmd_buf, sizeof(cmd_buf), _PING_FORMAT, dst, "-4");
        if (0 == _get_cmd_ret(cmd_buf, 1)) {
            return 1;
        }

        HY_MEMSET(cmd_buf, sizeof(cmd_buf));
        snprintf(cmd_buf, sizeof(cmd_buf), _PING_FORMAT, dst, "-6");
        if (0 == _get_cmd_ret(cmd_buf, 1)) {
            return 1;
        }
    }

    return 0;
}

void HyNetDestroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _net_context_s *context = *handle;

    HyThreadMutexLock_m(context->config_path_mutex_h);
    net_config_eth_save(&context->eth_c);
    net_config_wifi_save(&context->wifi_c);
    HyThreadMutexUnLock_m(context->config_path_mutex_h);
    HyThreadMutexDestroy(&context->config_path_mutex_h);

    net_wifi_destroy(&context->wifi_h);

    LOGI("net destroy, context: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *HyNetCreate(HyNetConfig_s *net_c)
{
    LOGT("net_c: %p \n", net_c);
    HY_ASSERT_RET_VAL(!net_c, NULL);

    _net_context_s *context = NULL;
    HyGpio_s gpio;

    do {
        context = HY_MEM_MALLOC_BREAK(_net_context_s *, sizeof(*context));
        HY_MEMCPY(&context->save_c, &net_c->save_c, sizeof(context->save_c));

        context->config_path_mutex_h = HyThreadMutexCreate_m();
        if (!context->config_path_mutex_h) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        if (0 != HyFileIsExist(context->save_c.config_path)) {
            if (!net_c->wifi_set_default_cb && !net_c->eth_set_default_cb) {
                LOGE("Please provide the default net configuration \n");
                break;
            }

            if (net_c->wifi_set_default_cb) {
                net_c->wifi_set_default_cb(&context->wifi_c,
                        &context->wifi_ip_info[0], net_c->args);
            }

            if (net_c->eth_set_default_cb) {
                net_c->eth_set_default_cb(&context->eth_c,
                        &context->eth_ip_info, net_c->args);
            }
        } else {
            HyThreadMutexLock_m(context->config_path_mutex_h);
            net_config_eth_load(&context->eth_c);
            net_config_wifi_load(&context->wifi_c);
            HyThreadMutexUnLock_m(context->config_path_mutex_h);
        }

        if (context->eth_c.enable) {
        } else {
            if (context->wifi_c.enable) {
                if (net_c->wifi_power_gpio) {
                    net_c->wifi_power_gpio(&gpio);
                }
                context->wifi_h = net_wifi_create_m(&gpio,
                        &context->wifi_c, context->wifi_ip_info);
                if (!context->wifi_h) {
                    LOGE("net_wifi_create_m failed \n");
                    break;
                }
            }
        }

        LOGI("net create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("net create failed \n");
    HyNetDestroy((void **)&context);
    return NULL;
}

