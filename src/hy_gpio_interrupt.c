/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_gpio_interrupt.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    26/04 2023 19:26
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        26/04 2023      create the file
 * 
 *     last modified: 26/04 2023 19:26
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_thread.h"
#include "hy_string.h"
#include "hy_gpio.h"
#include "hy_utils.h"

#include "hy_gpio_interrupt.h"

struct HyGpioInterrupt_s {
    HyGpioInterruptSaveConfig_s save_c;

    HyThread_s                  *thread_h;
    hy_s32_t                    fd;

    hy_s32_t                    is_exit;
};

static hy_s32_t _gpio_interrupt_loop_cb(void *args)
{
    HyGpioInterrupt_s *handle = args;
    HyGpioInterruptSaveConfig_s *save_c = &handle->save_c;
    hy_s32_t epfd;
    hy_s32_t nfds;
    struct epoll_event ev;
    struct epoll_event events[4];
    char val;

    epfd = epoll_create1(0);
    if (epfd < 0) {
        LOGES("epoll_create1 failed \n");
        return -1;
    }

    ev.data.fd = handle->fd;
    ev.events = EPOLLPRI | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, handle->fd, &ev);

    while (!handle->is_exit) {
        nfds=epoll_wait(epfd, events, HY_UTILS_ARRAY_CNT(events),
                        save_c->timeout_ms);
        if (nfds == 0) {
            LOGW("gpio interrupt epoll_wait timeout \n");

            if (save_c->gpio_interrupt_timeout_cb) {
                save_c->gpio_interrupt_timeout_cb(save_c->timeout_args);
            }
            continue;
        }

        for(hy_s32_t i = 0; i < nfds; ++i) {
            if (events[i].events & EPOLLPRI) {
                if (0 > lseek(events[i].data.fd, 0, SEEK_SET)) {//将读位置移动到头部
                    LOGES("lseek failed \n");
                    break;
                }

                if (0 > read(events[i].data.fd, &val, 1)) {
                    LOGES("read failed \n");
                    break;
                }

                if (save_c->gpio_interrupt_cb) {
                    save_c->gpio_interrupt_cb(val, save_c->args);
                }
            }
        }
    }

    close(epfd);

    return -1;
}

void HyGpioInterruptDestroy(HyGpioInterrupt_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyGpioInterrupt_s *handle = *handle_pp;

    handle->is_exit = 1;

    HyThreadDestroy(&handle->thread_h);

    close(handle->fd);

    // HyGpioExport(hy_u32_t gpio, HyGpioExport_e export)

    LOGI("HyGpioInterrupt destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyGpioInterrupt_s *HyGpioInterruptCreate(HyGpioInterruptConfig_s *gpio_interrupt_c)
{
    HY_ASSERT_RET_VAL(!gpio_interrupt_c, NULL);
    HyGpioInterrupt_s *handle = NULL;

    do {
        handle = HY_MEM_MALLOC_BREAK(HyGpioInterrupt_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &gpio_interrupt_c->save_c, sizeof(handle->save_c));

        HyGpio_s gpio;
        gpio.gpio = gpio_interrupt_c->gpio;
        gpio.direction = gpio_interrupt_c->direction;
        gpio.active_val = gpio_interrupt_c->active_val;
        gpio.trigger = gpio_interrupt_c->trigger;
        if (0 != HyGpioConfig(&gpio)) {
            LOGE("init gpio failed \n");
            break;
        }

        char buf[64] = {0};
        snprintf(buf, sizeof(buf), HY_GPIO_CLASS_PATH"/gpio%d/value", gpio.gpio);

        handle->fd = open(buf, O_RDONLY | O_NONBLOCK);
        if (handle->fd < 0) {
            LOGES("open failed, fd: %d \n", handle->fd);
            return NULL;
        }

        char val;
        read(handle->fd, &val, 1);

        HyThreadConfig_s thread_c;
        const char *thread_name = "gpio_interrupt_loop";
        HY_MEMSET(&thread_c, sizeof(thread_c));
        thread_c.save_c.args = handle;
        thread_c.save_c.policy = HY_THREAD_POLICY_SCHED_RR;
        thread_c.save_c.priority = 10;
        thread_c.save_c.thread_loop_cb = _gpio_interrupt_loop_cb;
        HY_STRNCPY(thread_c.save_c.name, HY_THREAD_NAME_LEN_MAX,
                   thread_name, HY_STRLEN(thread_name));
        handle->thread_h = HyThreadCreate(&thread_c);
        if (!handle->thread_h) {
            LOGE("HyThreadCreate failed \n");
            break;
        }

        LOGI("HyGpioInterrupt create, handle: %p \n", handle);
        return handle;
    } while(0);

    LOGE("HyGpioInterrupt create failed \n");
    HyGpioInterruptDestroy(&handle);
    return NULL;
}
