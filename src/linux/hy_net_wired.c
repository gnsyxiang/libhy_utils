/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_net_wired.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 11:18
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 11:18
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <linux/types.h>

#include "hy_net_wired.h"
#include "hy_fifo.h"

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_type.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_log.h"

#define ALONE_DEBUG 1

struct mii_data {
    hy_u16_t phy_id;
    hy_u16_t reg_num;
    hy_u16_t val_in;
    hy_u16_t val_out;
};

typedef struct {
    hy_s32_t led;
    hy_s32_t mode;
} _led_mode_t;

typedef struct {
    _led_mode_t led_mode;
    hy_s32_t    cnt;
    hy_s32_t    flag;
} _led_blink_mode_t;

typedef struct {
    HyNetWiredSaveConfig_t  save_config;

    void                    *led_thread_handle;
    void                    *led_fifo_handle;
    _led_blink_mode_t       led_blink_mode[HY_NET_WIRED_LED_MAX];

    hy_s32_t                exit_flag;
} _net_wired_context_t;

static _net_wired_context_t *context = NULL;

#define _MII_SET(fd, ifr, reg_val)                                      \
    do {                                                                \
        struct mii_data *mii;                                           \
                                                                        \
        mii = (struct mii_data *)&(ifr).ifr_data;                       \
        mii->reg_num    = (reg_val).reg;                                \
        mii->val_in     = (reg_val).val;                                \
                                                                        \
        if (ioctl(fd, SIOCSMIIREG, &ifr) < 0) {                         \
            LOGE("ioctl failed SIOCSMIIREG on %s \n", ifr.ifr_name);    \
        }                                                               \
    } while (0);

static int32_t _led_set(_led_mode_t *led_mode)
{
    int32_t skfd = -1;
    struct ifreq ifr;
    HyNetWiredLed_t *led = context->save_config.led[led_mode->led];
    HyNetWiredRegVal_t *reg_val = led[led_mode->mode].reg_val;

    if ((skfd = socket(AF_INET, SOCK_DGRAM,0)) < 0) {
        perror("socket");
        return -1;
    }

    strncpy(ifr.ifr_name, context->save_config.dev_name, IFNAMSIZ);
    if (ioctl(skfd, SIOCGMIIPHY, &ifr) < 0) {
        if (errno != ENODEV) {
            LOGE("ioctl failed SIOCGMIIPHY on '%s' \n",
                    context->save_config.dev_name);
            return -1;
        }
    }

    _MII_SET(skfd, ifr, reg_val[0]);
    _MII_SET(skfd, ifr, reg_val[1]);

    close(skfd);

    return 0;
}

void HyNetWiredSetLed(hy_s32_t led, hy_s32_t mode)
{
    _led_mode_t led_mode;

    led_mode.led = led;
    led_mode.mode = mode;

    HyFifoWrite(context->led_fifo_handle, &led_mode, sizeof(led_mode));
}

static hy_s32_t _led_loop_cb(void *args)
{
    size_t val = 0;
    _led_mode_t led_mode;
    hy_s32_t i;
    _led_blink_mode_t *led_blink_mode = &context->led_blink_mode[0];
    hy_s32_t judge_condition[HY_NET_WIRED_LED_MAX][2] = {
        {HY_NET_WIRED_LED_MODE_SLOW_BLINK,  5},
        {HY_NET_WIRED_LED_MODE_FAST_BLINK,  3},
    };

    while (!context->exit_flag) {
        val = 0;
        while (0 == val) {
            val = HyFifoGetInfo(context->led_fifo_handle, HY_FIFO_INFO_USED_LEN);
            usleep(100 * 1000);

            for (i = 0; i < HY_NET_WIRED_LED_MAX; ++i) {
                led_blink_mode[i].cnt++;
                led_mode.led = i;

                if (led_blink_mode[i].led_mode.mode == judge_condition[i][0]
                        && led_blink_mode[i].cnt == judge_condition[i][1]) {
                    led_blink_mode[i].cnt = 0;

                    if (led_blink_mode[i].flag) {
                        led_mode.mode = HY_NET_WIRED_LED_MODE_OFF;
                    } else {
                        led_mode.mode = HY_NET_WIRED_LED_MODE_ON;
                    }
                    led_blink_mode[i].flag = !led_blink_mode[i].flag;
                    _led_set(&led_mode);
                }
            }
        }

        HyFifoRead(context->led_fifo_handle, &led_mode, sizeof(led_mode));

        for (i = 0; i < HY_NET_WIRED_LED_MAX; ++i) {
            if (led_mode.led == i) {
                HY_MEMCPY(&context->led_blink_mode[i].led_mode,
                        &led_mode, sizeof(led_mode));
            }
        }

        if (led_mode.mode == HY_NET_WIRED_LED_MODE_OFF
                || led_mode.mode == HY_NET_WIRED_LED_MODE_ON) {
            _led_set(&led_mode);
        }
    }

    return -1;
}

void HyNetWiredDestroy(void **handle)
{
    context->exit_flag = 1;

    HyThreadDestroy(&context->led_thread_handle);

    HyFifoDestroy(&context->led_fifo_handle);

    HY_MEM_FREE_PP(&context);

    LOGI("net wired destroy successful \n");
}

void *HyNetWiredCreate(HyNetWiredConfig_t *config)
{
    HY_ASSERT_VAL_RET_VAL(!config, NULL);

    do {
        context = HY_MEM_MALLOC_BREAK(_net_wired_context_t *, sizeof(*context));
        HY_MEMCPY(&context->save_config, &config->save_config, sizeof(config->save_config));

        HyFifoConfig_t led_fifo_config;
        led_fifo_config.save_config.len = sizeof(_led_mode_t) * 6;
        context->led_fifo_handle = HyFifoCreate(&led_fifo_config);
        if (!context->led_fifo_handle) {
            LOGE("HyFifoCreate failed \n");
            break;
        }

        context->led_thread_handle = HyThreadCreate_m("net_led", _led_loop_cb, context);
        if (!context->led_thread_handle) {
            LOGE("HyThreadCreate failed \n");
            break;
        }

        LOGI("net wired create successful \n");
        return context;
    } while (0);

    HyNetWiredDestroy(NULL);
    return NULL;
}

