/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_gpio_isr.c
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
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "hy_assert.h"
#include "hy_thread.h"
#include "hy_pipe.h"
#include "hy_mem.h"
#include "hy_gpio.h"
#include "hy_string.h"

#include "hy_gpio_isr.h"

#define _PIPE_EXIT 'Q'

struct HyGpioIsr_s {
    HyGpioIsrSaveConfig_s   save_c;
    hy_s32_t                is_exit;

    hy_s32_t                fd;
    HyThread_s              *thread_h;
    HyPipe_s                *pipe_h;
};

static hy_s32_t _gpio_isr_loop_cb(void *args)
{
    HyGpioIsr_s *handle = (HyGpioIsr_s *)args;
    HyGpioIsrSaveConfig_s *save_c = &handle->save_c;
    struct epoll_event events;
    struct epoll_event ev;
    hy_s32_t epfd;
    hy_s32_t nfds;
    char val;
    hy_s32_t pipe_fd = HyPipeReadFdGet(handle->pipe_h);

    epfd = epoll_create1(0);
    if (epfd < 0) {
        LOGES("epoll_create1 failed \n");
        return -1;
    }

    HY_MEMSET(&ev, sizeof(ev));
    ev.data.fd  = handle->fd;
    ev.events   = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, handle->fd, &ev);

    HY_MEMSET(&ev, sizeof(ev));
    ev.data.fd  = pipe_fd;
    ev.events   = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, pipe_fd, &ev);

    while (!handle->is_exit) {
        nfds = epoll_wait(epfd, &events, 1, save_c->timeout_ms);
        if (nfds == 0) {
            LOGW("gpio isr epoll_wait timeout(%dms) \n", save_c->timeout_ms);

            if (save_c->gpio_isr_timeout_cb) {
                save_c->gpio_isr_timeout_cb(save_c->args);
            }
            continue;
        }

        for (hy_s32_t i = 0; i < nfds; i++) {
            if (events.data.fd == handle->fd) {
                if (0 > lseek(events.data.fd, 0, SEEK_SET)) {//将读位置移动到头部
                    LOGES("lseek failed \n");
                    break;
                }

                if (0 > read(events.data.fd, &val, 1)) {
                    LOGES("read failed \n");
                    break;
                }

                if (save_c->gpio_isr_cb) {
                    save_c->gpio_isr_cb(val, save_c->args);
                }
            } else if (events.data.fd == pipe_fd) {
                HyPipeRead(handle->pipe_h, &val, sizeof(val));
                if (val == _PIPE_EXIT) {
                    LOGI("exit gpio_isr loop \n");
                }
            }
        }
    }

    close(epfd);

    return -1;
}

void HyGpioIsrSetTimeout(HyGpioIsr_s *handle, hy_u32_t timeout_ms)
{
    HY_ASSERT_RET(!handle);

    LOGI("set timeout: %d \n", timeout_ms);
    handle->save_c.timeout_ms = timeout_ms;
}

void HyGpioIsrDestroy(HyGpioIsr_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyGpioIsr_s *handle = *handle_pp;
    char op;

    handle->is_exit = 1;

    op = _PIPE_EXIT;
    HyPipeWrite(handle->pipe_h, &op, sizeof(op));

    HyThreadDestroy(&handle->thread_h);

    close(handle->fd);

    HyPipeDestroy(&handle->pipe_h);

    // HyGpioExport(hy_u32_t gpio, HyGpioExport_e export)

    LOGI("HyGpioIsr destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyGpioIsr_s *HyGpioIsrCreate(HyGpioIsrConfig_s *gpio_isr_c)
{
    HY_ASSERT_RET_VAL(!gpio_isr_c, NULL);

    const char *thread_name = "gpio_isr_loop";
    HyThreadConfig_s thread_c;
    HyGpioIsr_s *handle = NULL;
    char buf[64] = {0};
    HyGpio_s gpio;
    char val;

    do {
        handle = HY_MEM_MALLOC_BREAK(HyGpioIsr_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &gpio_isr_c->save_c, sizeof(handle->save_c));

        handle->pipe_h = HyPipeCreate_m(HY_PIPE_BLOCK_STATE_BLOCK);
        if (!handle->pipe_h) {
            LOGE("HyPipeCreate_m failed \n");
            break;
        }

        gpio.gpio       = gpio_isr_c->gpio;
        gpio.direction  = gpio_isr_c->direction;
        gpio.active_val = gpio_isr_c->active_val;
        gpio.trigger    = gpio_isr_c->trigger;
        if (0 != HyGpioConfig(&gpio)) {
            LOGE("init gpio failed \n");
            break;
        }

        snprintf(buf, sizeof(buf), HY_GPIO_CLASS_PATH"/gpio%d/value", gpio.gpio);

        handle->fd = open(buf, O_RDONLY | O_NONBLOCK);
        if (handle->fd < 0) {
            LOGES("open failed, fd: %d \n", handle->fd);
            break;
        }

        read(handle->fd, &val, 1);

        HY_MEMSET(&thread_c, sizeof(thread_c));
        thread_c.save_c.args            = handle;
        thread_c.save_c.policy          = HY_THREAD_POLICY_SCHED_RR;
        thread_c.save_c.priority        = 10;
        thread_c.save_c.thread_loop_cb  = _gpio_isr_loop_cb;
        HY_STRNCPY(thread_c.save_c.name, HY_THREAD_NAME_LEN_MAX,
                   thread_name, HY_STRLEN(thread_name));
        handle->thread_h = HyThreadCreate(&thread_c);
        if (!handle->thread_h) {
            LOGE("HyThreadCreate failed \n");
            break;
        }

        LOGI("HyGpioIsr create, handle: %p \n", handle);
        return handle;
    } while(0);

    LOGE("HyGpioIsr create failed \n");
    HyGpioIsrDestroy(&handle);
    return NULL;
}
