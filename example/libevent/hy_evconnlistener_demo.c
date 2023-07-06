/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_evconnlistener_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    11/05 2023 10:05
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        11/05 2023      create the file
 * 
 *     last modified: 11/05 2023 10:05
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <hy_log/hy_log.h>

#include <event2/event.h>
#include <event2/listener.h>

#include "config.h"

#include "hy_type.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_signal.h"
#include "hy_module.h"
#include "hy_utils.h"

#define _APP_NAME       "hy_evconnlistener_demo"

#define _SERVER_IP      "0.0.0.0"
#define _SERVER_PORT    (1279)

typedef struct {
    hy_s32_t                is_exit;

    struct event_base       *evlistener_base;
    struct evconnlistener   *listener;
    struct event            *time_1s_event;
} _main_context_s;

static void _signal_error_cb(void *args)
{
    _main_context_s *context = args;
    context->is_exit = 1;

    LOGE("------error cb\n");
}

static void _signal_user_cb(void *args)
{
    _main_context_s *context = args;
    context->is_exit = 1;

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
    log_c.config_file               = "../res/hy_log/zlog.conf";
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
    _main_context_s *context = *context_pp;

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

static void _time_1s_event_cb(evutil_socket_t sock, short events, void *args)
{
    LOGI("time 1s \n");
}

static void _listener_cb(struct evconnlistener *listener, evutil_socket_t sock,
                              struct sockaddr *addr, int socklen, void *args)
{
    LOGI("sock: %d \n", sock);
    close(sock);
}

static void _evconnlistener_destroy(_main_context_s *context)
{
    evconnlistener_free(context->listener);

    event_free(context->time_1s_event);

    event_base_free(context->evlistener_base);
}

static hy_s32_t _evconnlistener_create(_main_context_s *context)
{
    do {
        struct event_config *cfg = event_config_new();
        event_config_require_features(cfg,  EV_FEATURE_ET | EV_FEATURE_O1);
        event_config_set_flag(cfg, EVENT_BASE_FLAG_NOLOCK
                              | EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST);
        context->evlistener_base = event_base_new_with_config(cfg);
        if (!context->evlistener_base) {
            LOGE("don't support this features\n");

            context->evlistener_base = event_base_new();
        }
        if (!context->evlistener_base) {
            LOGE("event_base new failed \n");
            break;
        }
        LOGI("current method: %s\n",
             event_base_get_method(context->evlistener_base));

        context->time_1s_event = event_new(context->evlistener_base, -1,
                                           EV_PERSIST, _time_1s_event_cb, context);
        if (!context->time_1s_event) {
            LOGE("event_new time 1s event failed \n");
            break;
        }
        struct timeval tv_1s = {1, 0};
        evtimer_add(context->time_1s_event, &tv_1s);

        struct sockaddr_in addr;
        memset(&addr, 0x0, sizeof(addr));
        addr.sin_family = AF_INET;
        // addr.sin_addr.s_addr = inet_addr(_SERVER_IP);
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(_SERVER_PORT);
        context->listener = evconnlistener_new_bind(context->evlistener_base,
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

        return 0;
    } while(0);

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

        event_base_dispatch(context->evlistener_base);

        while (!context->is_exit) {
            sleep(1);
        }
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

