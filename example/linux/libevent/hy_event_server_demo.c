/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_event_server_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    31/08 2023 15:27
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        31/08 2023      create the file
 * 
 *     last modified: 31/08 2023 15:27
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include <hy_log/hy_log.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

#include "config.h"

#include "hy_type.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_signal.h"
#include "hy_assert.h"
#include "hy_module.h"
#include "hy_utils.h"
#include "collect_bends.h"

#define _APP_NAME       "hy_event_server_demo"

#define _SERVER_IP      "0.0.0.0"

// #define _TEST_SERVER
#define _TEST_LOG

#ifdef _TEST_SERVER
#define _SERVER_PORT    (30000)
#endif

#ifdef _TEST_LOG
#define _SERVER_PORT    (30001)
#endif

typedef struct {
    hy_s32_t                is_exit;

    struct event_base       *base;
    struct evconnlistener   *listener;
} _main_context_s;

typedef struct {
    struct evbuffer *evbuf;
    _main_context_s *context;
} _client_data_s;

static void _signal_error_cb(void *args)
{
    _main_context_s *context = args;
    context->is_exit = 1;
    event_base_loopexit(context->base, NULL);

    LOGE("------error cb\n");
}

static void _signal_user_cb(void *args)
{
    _main_context_s *context = args;
    context->is_exit = 1;
    event_base_loopexit(context->base, NULL);

    LOGW("------user cb\n");
}

static void _bool_module_destroy(_main_context_s **context_pp)
{
    HyModuleDestroyBool_s bool_module[] = {
        {"signal",          HySignalDestroy },
        {"log",             HyLogDeInit     },
    };

    HY_MODULE_RUN_DESTROY_BOOL(bool_module);
}

static hy_s32_t _bool_module_create(_main_context_s *context)
{
    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(log_c));
#ifdef _TEST_SERVER
    log_c.config_file               = "../res/hy_log/zlog.conf";
#endif
#ifdef _TEST_LOG
    log_c.config_file               = "./zlog-log.conf";
#endif
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.level              = HY_LOG_LEVEL_INFO;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL_NO_PID_ID;

    int8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    int8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGINT, SIGTERM, SIGUSR1, SIGUSR2,
    };

    HySignalConfig_t signal_c;
    HY_MEMSET(&signal_c, sizeof(signal_c));
    HY_MEMCPY(signal_c.error_num, signal_error_num, sizeof(signal_error_num));
    HY_MEMCPY(signal_c.user_num, signal_user_num, sizeof(signal_user_num));
    signal_c.save_c.app_name        = _APP_NAME;
    signal_c.save_c.coredump_path   = "./";
    signal_c.save_c.error_cb        = _signal_error_cb;
    signal_c.save_c.user_cb         = _signal_user_cb;
    signal_c.save_c.args            = context;

    HyModuleCreateBool_s bool_module[] = {
        {"log",         &log_c,         (HyModuleCreateBoolCb_t)HyLogInit,          HyLogDeInit},
        {"signal",      &signal_c,      (HyModuleCreateBoolCb_t)HySignalCreate,     HySignalDestroy},
    };

    HY_MODULE_RUN_CREATE_BOOL(bool_module);
}

static void _handle_module_destroy(_main_context_s **context_pp)
{
    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {NULL, NULL, NULL},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_s *context)
{
    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {NULL, NULL, NULL, NULL, NULL},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

static void _listener_error_cb(struct evconnlistener *listener, void *args)
{
    hy_s32_t err = EVUTIL_SOCKET_ERROR();
    struct event_base *base = evconnlistener_get_base(listener);

    LOGE("error listener: %d: %s \n", err, evutil_socket_error_to_string(err));

    event_base_loopexit(base, NULL);
}

static void _client_read_cb(struct bufferevent *bev, void *args)
{
    char buf[5 * 1024];
    hy_s32_t ret;

#ifdef _TEST_LOG
    do {
        HY_MEMSET(buf, sizeof(buf));
        ret = bufferevent_read(bev, buf, sizeof(buf) - 1);
        if (ret > 0) {
            LOGI("%s\n", buf);
        }
    } while(ret > 0);
#else
    _client_data_s *client_data = args;

    while (ret = bufferevent_read(bev, buf, sizeof(buf)), ret > 0) {
        evbuffer_add(client_data->evbuf, buf, ret);
    }

    protocol_handle_frame(bev, client_data->evbuf);
#endif

}

static void _client_event_cb(struct bufferevent *bev, short event, void *args)
{
    _client_data_s *client_data = args;

    HySocketInfo_s socket_info;
    protocol_get_ip(bev, &socket_info);

    if (BEV_EVENT_EOF & event) {
        LOGE("client disconnect, ip: %s \n", socket_info.ip);
    } else if(BEV_ERROR & event) {
        LOGE("Error occured.\n");
    }

    if (client_data) {
        if (client_data->evbuf) {
            evbuffer_free(client_data->evbuf);
        }

        HY_MEM_FREE_PP(&client_data);
    }

    bufferevent_free(bev);
}

static void _listener_cb(struct evconnlistener *listener, evutil_socket_t sock,
                              struct sockaddr *addr, int socklen, void *args)
{
    _main_context_s *context = args;
    struct bufferevent *bev;
    char *ip;

    ip = inet_ntoa(((struct sockaddr_in *)addr)->sin_addr);
    LOGW("new client connect, ip: %s \n", ip);

    _client_data_s *client_data = HY_MEM_CALLOC_RETURN(_client_data_s *, sizeof(*client_data));
    client_data->context = context;

    client_data->evbuf = evbuffer_new();
    if (!client_data->evbuf) {
        LOGE("evbuffer_new failed \n");
        close(sock);
        return;
    }

    bev = bufferevent_socket_new(context->base, sock, BEV_OPT_CLOSE_ON_FREE);
    // bufferevent_setwatermark(bev, EV_READ, 32, 0);
    bufferevent_setcb(bev, _client_read_cb, NULL, _client_event_cb, client_data);
    bufferevent_enable(bev, EV_READ);
    bufferevent_enable(bev, EV_WRITE);
}

static void _evconnlistener_destroy(_main_context_s *context)
{
    evconnlistener_free(context->listener);

    event_base_free(context->base);
}

static hy_s32_t _evconnlistener_create(_main_context_s *context)
{
    struct event_config *cfg;

    do {
        cfg = event_config_new();
        event_config_require_features(cfg,  EV_FEATURE_ET | EV_FEATURE_O1);
        event_config_set_flag(cfg, EVENT_BASE_FLAG_NOLOCK
                              | EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST);
        context->base = event_base_new_with_config(cfg);
        if (!context->base) {
            LOGE("don't support this features\n");

            context->base = event_base_new();
        }
        if (!context->base) {
            LOGE("event_base new failed \n");
            break;
        }
        LOGI("current method: %s\n", event_base_get_method(context->base));

        struct sockaddr_in addr;
        memset(&addr, 0x0, sizeof(addr));
        addr.sin_family = AF_INET;
        // addr.sin_addr.s_addr = inet_addr(_SERVER_IP);
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(_SERVER_PORT);
        context->listener = evconnlistener_new_bind(context->base,
                                                    _listener_cb,
                                                    context,
                                                    LEV_OPT_CLOSE_ON_FREE
                                                    | LEV_OPT_REUSEABLE
                                                    | LEV_OPT_DEFERRED_ACCEPT,
                                                    -1,
                                                    (struct sockaddr*)(&addr),
                                                    sizeof(addr));
        if (!context->listener) {
            LOGE("evconnlistener_new_bind failed \n");
            break;
        }

        evconnlistener_set_error_cb(context->listener, _listener_error_cb);

        event_config_free(cfg);
        return 0;
    } while(0);

    event_config_free(cfg);

    return -1;
}

int main(int argc, char *argv[])
{
    _main_context_s *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_s *, sizeof(*context));

        struct {
            const char *name;
            hy_s32_t (*create)(_main_context_s *context);
        } create_arr[] = {
            {"_bool_module_create",     _bool_module_create},
            {"_handle_module_create",   _handle_module_create},
        };
        for (size_t i = 0; i < HY_UTILS_ARRAY_CNT(create_arr); i++) {
            if (create_arr[i].create) {
                if (0 != create_arr[i].create(context)) {
                    LOGE("%s failed \n", create_arr[i].name);
                }
            }
        }

        if (0 != _evconnlistener_create(context)) {
            LOGE("_evconnlistener_create failed \n");
            break;
        }

        LOGE("version: %s, data: %s, time: %s \n", VERSION, __DATE__, __TIME__);

        event_base_dispatch(context->base);
    } while (0);

    _evconnlistener_destroy(context);
    void (*destroy_arr[])(_main_context_s **context_pp) = {
        _handle_module_destroy,
        _bool_module_destroy
    };
    for (size_t i = 0; i < HY_UTILS_ARRAY_CNT(destroy_arr); i++) {
        if (destroy_arr[i]) {
            destroy_arr[i](&context);
        }
    }
    HY_MEM_FREE_PP(&context);

    return 0;
}
