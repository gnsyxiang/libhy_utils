/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_event_listener.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    27/04 2023 09:16
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        27/04 2023      create the file
 * 
 *     last modified: 27/04 2023 09:16
 */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "hy_mem.h"
#include "hy_assert.h"
#include "hy_utils.h"

#include "hy_event_listener.h"

struct HyEventListener_s {
    HyEventListenerSaveConfig_s save_c;

    struct event_base           *listener_base;
    struct evconnlistener       *listener;
    struct event                **signal_event;
};

static void _listener_error_cb(struct evconnlistener *listener, void *args)
{
    HyEventListener_s *handle = args;
    hy_s32_t err = EVUTIL_SOCKET_ERROR();
    struct event_base *base = evconnlistener_get_base(listener);

    LOGE("error listener: %d: %s \n", err, evutil_socket_error_to_string(err));

    if (handle->save_c.listener_error_cb) {
        handle->save_c.listener_error_cb(handle->save_c.listener_args);
    }

    event_base_loopexit(base, NULL);
}

static void _listener_cb(struct evconnlistener *listener, evutil_socket_t sock,
                              struct sockaddr *addr, int socklen, void *args)
{
    HyEventListener_s *handle = args;

    if (handle->save_c.listener_cb) {
        handle->save_c.listener_cb(sock, addr, socklen,
                                   handle->save_c.listener_args);
    }
}

void HyEventListenerExit(HyEventListener_s *handle)
{
    HY_ASSERT_RET(!handle);

    struct timeval delay = { 2, 0 };
    event_base_loopbreak(handle->listener_base);
    event_base_loopexit(handle->listener_base, &delay);
}

void HyEventListenerLoop(HyEventListener_s *handle)
{
    HY_ASSERT_RET(!handle);

    event_base_dispatch(handle->listener_base);
}

void HyEventListenerDestroy(HyEventListener_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyEventListener_s *handle = *handle_pp;

    evconnlistener_free(handle->listener);

    for (size_t i = 0; i < handle->save_c.signal_cb_num; i++) {
        event_free(handle->signal_event[i]);
    }

    event_base_free(handle->listener_base);

    HY_MEM_FREE_PP(handle_pp);
}

HyEventListener_s *HyEventListenerCreate(HyEventListenerConfig_s *listener_c)
{
    HY_ASSERT_RET_VAL(!listener_c, NULL);

    HyEventListener_s *handle = NULL;
    HyEventListenerSaveConfig_s *save_c = NULL;

    do {
        handle = HY_MEM_MALLOC_BREAK(HyEventListener_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &listener_c->save_c, sizeof(handle->save_c));

        save_c = &handle->save_c;

        struct event_config *cfg = event_config_new();
        event_config_require_features(cfg,  EV_FEATURE_ET | EV_FEATURE_O1);
        event_config_set_flag(cfg, EVENT_BASE_FLAG_NOLOCK
                              | EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST);

        handle->listener_base = event_base_new_with_config(cfg);
        if (!handle->listener_base) {
            LOGE("don't support this features\n");

            handle->listener_base = event_base_new();
        }
        if (!handle->listener_base) {
            LOGE("event_base new failed \n");
        }
        LOGI("current method: %s\n", event_base_get_method(handle->listener_base));

        handle->signal_event = HY_MEM_MALLOC_BREAK(struct event **,
                                                   sizeof(struct event *) * save_c->signal_cb_num);
        for (size_t i = 0; i < save_c->signal_cb_num; i++) {

            handle->signal_event[i] = evsignal_new(handle->listener_base,
                                                   save_c->signal_cb[i].signal,
                                                   save_c->signal_cb[i].signal_cb,
                                                   save_c->signal_cb[i].signal_cb_args); 
            if (!handle->signal_event[i]) {
                LOGE("evsignal_new failed \n");
            }

            if (event_add(handle->signal_event[i], 0) != 0) {
                LOGE("event_add failed \n");
            }
        }

        struct sockaddr_in addr;
        memset(&addr, 0x0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(listener_c->ip);
        addr.sin_port = htons(listener_c->port);
        handle->listener = evconnlistener_new_bind(handle->listener_base,
                                                   _listener_cb,
                                                   handle,
                                                   LEV_OPT_CLOSE_ON_FREE
                                                   | LEV_OPT_REUSEABLE
                                                   | LEV_OPT_DEFERRED_ACCEPT,
                                                   -1,
                                                   (struct sockaddr*)(&addr),
                                                   sizeof(addr));
        if (!handle->listener) {
            LOGE("evconnlistener_new_bind failed \n");
        }

        evconnlistener_set_error_cb(handle->listener, _listener_error_cb);

        LOGI("HyEventListener create, handle: 0x%p \n", handle);
        return handle;
    } while(0);

    LOGE("HyEventListener create failed \n");
    return NULL;
}
