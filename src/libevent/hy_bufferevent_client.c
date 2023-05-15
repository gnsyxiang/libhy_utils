/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_bufferevent_client.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    04/05 2023 17:31
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        04/05 2023      create the file
 * 
 *     last modified: 04/05 2023 17:31
 */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_thread.h"

#include "hy_bufferevent_client.h"
#include "hy_type.h"

struct HyBuffereventClient_s {
    HyBuffereventClientSaveConfig_s save_c;

    hy_s32_t            is_exit;
    hy_s32_t            sock;
    struct event_base   *base;
    struct bufferevent  *event;

    HyThread_s          *read_thread_h;
};

static void _read_cb(struct bufferevent *bev, void *args)
{
    HyBuffereventClient_s *handle = args;
    char buf[1024] = {0};
    hy_s32_t len = 0;

    len = bufferevent_read(bev, buf, 1024);

    if (handle && handle->save_c.read_cb) {
        handle->save_c.read_cb(buf, len, handle->save_c.args);
    }
}

static void _write_cb(struct bufferevent *bev, void *args)
{

}

static void _event_cb(struct bufferevent *bev, short events, void *args)
{
    HY_ASSERT_RET(!args);

    HyBuffereventClient_s *handle = args;
    hy_s32_t flag = 0;

    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        handle->is_exit = 1;
        flag = HY_BUFFEREVENT_CLIENT_FLAG_DISCONNECTED;
        bufferevent_free(bev);
    } else if (events & BEV_EVENT_CONNECTED) {
        flag = HY_BUFFEREVENT_CLIENT_FLAG_CONNECTED;
    }

    if (handle->save_c.event_cb) {
        handle->save_c.event_cb(flag, handle->save_c.args);
    }
}

static hy_s32_t _read_thread_loop_cb(void *args)
{
    HyBuffereventClient_s *handle = args;
    hy_s32_t len = 0;
    struct sockaddr_in addr;

    while (!handle->is_exit) {
        HY_MEMSET(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        inet_aton(handle->save_c.ip, &addr.sin_addr);
        addr.sin_port = htons(handle->save_c.port);
        len = bufferevent_socket_connect(handle->event,
                                            (struct sockaddr*)(&addr), sizeof(addr));
        if (0 != len) {
            LOGW("bufferevent_socket_connect failed \n");
            sleep(2);
            continue;
        }

        event_base_dispatch(handle->base);
    }

    return -1;
}

hy_s32_t HyBuffereventClientWrite(HyBuffereventClient_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);
    hy_s32_t ret;

    ret = bufferevent_write(handle->event, buf, len);

    return ret;
}

void HyBuffereventClientDestroy(HyBuffereventClient_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyBuffereventClient_s *handle= *handle_pp;

    event_base_free(handle->base);

    close(handle->sock);

    HyThreadDestroy(&handle->read_thread_h);

    LOGI("buffereventclient destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyBuffereventClient_s *HyBuffereventClientCreate(HyBuffereventClientConfig_s *client_c)
{
    HY_ASSERT_RET_VAL(!client_c, NULL);

    HyBuffereventClient_s *handle = NULL;
    do {
        handle = HY_MEM_CALLOC_BREAK(HyBuffereventClient_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &client_c->save_c, sizeof(client_c->save_c));

        handle->base = event_base_new();
        if (!handle->base) {
            LOGE("event_base_new failed \n");
            break;
        }

        handle->sock = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == handle->sock) {
            LOGE("socket failed \n");
            break;
        }
        evutil_make_socket_nonblocking(handle->sock);

        handle->event = bufferevent_socket_new(handle->base,
                                               handle->sock, BEV_OPT_CLOSE_ON_FREE);
        if (!handle->event) {
            LOGE("bufferevent_socket_new failed \n");
            break;
        }
        bufferevent_enable(handle->event, EV_READ | EV_WRITE);
        bufferevent_setcb(handle->event, _read_cb, _write_cb, _event_cb, handle);

        handle->read_thread_h = HyThreadCreate_m(
            "ben_client_read", _read_thread_loop_cb, handle);
        if (!handle->read_thread_h) {
            LOGE("HyThreadCreate failed \n");
            break;
        }

        LOGI("buffereventclient create, handle: %p \n", handle);
        return handle;
    } while(0);

    LOGE("buffereventclient create failed \n");
    return NULL;
}
