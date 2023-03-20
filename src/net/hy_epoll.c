/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_epoll.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/03 2023 11:21
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/03 2023      create the file
 * 
 *     last modified: 18/03 2023 11:21
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "log/hy_log.h"

#include "hy_mem.h"
#include "hy_assert.h"

#include "hy_epoll.h"

typedef struct {
    HyEpollSaveConfig_s     save_c;

    hy_s32_t                fd;                 ///< epoll文件描述符

    pthread_t               id;                 ///< 线程id
    hy_s32_t                is_exit;            ///< 控制线程退出
    hy_s32_t                pipe_fd[2];         ///< pipe文件描述符，控制线程退出
    hy_s32_t                wait_exit_flag;     ///< 控制线程退出标志位
} _epoll_context_s;

static void *_epoll_thread_cb(void *args)
{
    hy_s32_t cnt = 0;
    hy_u32_t len = 0;
    _epoll_context_s *context = args;
    HyEpollSaveConfig_s *save_c = &context->save_c;
    HyEpollEventCbParam_s *cb_param = NULL;
    struct epoll_event *events = NULL;

    len = save_c->max_event * sizeof(struct epoll_event);
    events = calloc(1, len);
    if (!events) {
        LOGE("calloc failed \n");
        return NULL;
    }

    while (!context->is_exit) {
        memset(events, '\0', len);
        cnt = epoll_wait(context->fd, events, save_c->max_event, -1);
        if (-1 == cnt) {
            LOGE("epoll_wait failed \n");
            break;
        }
        // LOGI("epoll_wait cnt: %d \n", cnt);

        for (hy_s32_t i = 0; i < cnt; ++i) {
            cb_param = events[i].data.ptr;
            if (-1 == epoll_ctl(context->fd, EPOLL_CTL_DEL, cb_param->fd, NULL)) {
                LOGES("epoll_ctl failed \n");
                continue;
            }

            if (cb_param->fd == context->pipe_fd[0]) {
                LOGI("exit epoll wait \n");
                goto _L_EPOLL_1;
            }

            if (save_c->event_epoll_cb) {
                save_c->event_epoll_cb(events[i].data.ptr);
            }
        }
    }

_L_EPOLL_1:
    context->wait_exit_flag = 1;

    HY_MEM_FREE_PP(&events);

    return NULL;
}

hy_s32_t HyEpollAdd(void *handle, hy_s32_t event, hy_s32_t fd,
                    HyEpollEventCbParam_s *cb_param)
{
    HY_ASSERT_RET_VAL(!handle, -1);
    HY_ASSERT_RET_VAL(!cb_param, -1);

    _epoll_context_s *context = handle;
    struct epoll_event ev;

    HY_MEMSET(&ev, sizeof(ev));
    ev.events   = event;
    ev.data.ptr = cb_param;
    return epoll_ctl(context->fd, EPOLL_CTL_ADD, fd, &ev);
}

hy_s32_t HyEpollDel(void *handle, hy_s32_t fd)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    _epoll_context_s *context = handle;

    return epoll_ctl(context->fd, EPOLL_CTL_DEL, fd, NULL);
}

void HyEpollDestroy(void **handle_pp)
{
    HY_ASSERT_RET(!handle_pp);
    HY_ASSERT_RET(!*handle_pp);

    hy_s32_t cnt = 0;
    _epoll_context_s *context = *handle_pp;
    LOGI("epoll context: %p destroy, thread_id: 0x%lx, "
            "epoll_fd: %d, pipe_fd[0]: %d, pipe_fd[1]: %d \n",
            context, context->id, context->fd,
            context->pipe_fd[0], context->pipe_fd[1]);

    context->is_exit = 1;
    write(context->pipe_fd[1], context, sizeof(*context));
    while (!context->wait_exit_flag) {
        usleep(10 * 1000);

        if (cnt++ > 100) {
            LOGW("wait pthread exit failed \n");
            break;
        }
    }
    pthread_join(context->id, NULL);

    close(context->pipe_fd[0]);
    close(context->pipe_fd[1]);

    close(context->fd);

    HY_MEM_FREE_PP(handle_pp);
}

void *HyEpollCreate(HyEpollConfig_s *config)
{
    HY_ASSERT_RET_VAL(!config, NULL);

    _epoll_context_s *context = NULL;
    do {
        context = HY_MEM_CALLOC_BREAK(_epoll_context_s *, sizeof(*context));

        memcpy(&context->save_c,
               &config->save_c, sizeof(context->save_c));

        context->fd = epoll_create1(0);
        if (-1 == context->fd) {
            LOGE("epoll_create1 failed \n");
            break;
        }

        if (0 != pipe(context->pipe_fd)) {
            LOGE("pipe failed \n");
            break;
        }

        struct epoll_event ev;
        HY_MEMSET(&ev, sizeof(ev));
        ev.events = EPOLLIN | EPOLLET;
        ev.data.ptr = &context->pipe_fd[0];
        if (-1 == epoll_ctl(context->fd, EPOLL_CTL_ADD, context->pipe_fd[0], &ev)) {
            LOGE("epoll_ctl failed \n");
            break;
        }

        if (0 != pthread_create(&context->id, NULL, _epoll_thread_cb, context)) {
            LOGE("pthread_create failed \n");
            break;
        }

        LOGI("epoll context: %p create, thread_id: 0x%lx, "
             "epoll_fd: %d, pipe_fd[0]: %d, pipe_fd[1]: %d \n",
             context, context->id, context->fd,
             context->pipe_fd[0], context->pipe_fd[1]);

        return context;
    } while(1);

    LOGE("epoll context: %p create failed \n", context);
    HyEpollDestroy((void **)&context);

    return NULL;
}

