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
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <hy_log/hy_log.h>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

#include "config.h"

#include "hy_mem.h"
#include "hy_string.h"
#include "hy_signal.h"
#include "hy_assert.h"
#include "hy_module.h"
#include "hy_utils.h"
#include "hy_thread_pool.h"
#include "hy_thread_mutex.h"
#include "hy_package_list.h"

#define _APP_NAME       "hy_evconnlistener_demo"

#define _SERVER_IP      "0.0.0.0"
#define _SERVER_PORT    (1279)

#define _TEST_1S_EVENT

static const char *ip_arr[] = {
    "192.168.0.17", "192.168.0.18", "192.168.0.19", "192.168.0.20", "192.168.0.252",
};

typedef struct {
    hy_s32_t                is_exit;

    struct event_base       *evlistener_base;
    struct evconnlistener   *listener;
#ifdef _TEST_1S_EVENT
    struct event            *time_1s_event;
#endif

    HyPackageList_s         *client_plist_h;
    HyThreadPool_s          *thread_pool_h;
    HyThreadMutex_s         *client_mutext_h[10];
    hy_u32_t                client_cnt[10];

    hy_u32_t                cnt;
} _main_context_s;

typedef struct {
    _main_context_s         *context;
    hy_s32_t                fd;
    char                    ip[16];
} _client_node_s;

static void _client_node_destroy(HyPackageListNode_s **list_node_pp)
{
    HY_ASSERT_RET(!list_node_pp || !*list_node_pp);
    HyPackageListNode_s *list_node = *list_node_pp;

    _client_node_s *client_node = list_node->user_data;

    if (client_node) {
        HY_MEM_FREE_PP(&client_node);
    }

    HY_MEM_FREE_PP(list_node_pp);
}

static HyPackageListNode_s *_client_node_create(void)
{
    HyPackageListNode_s *list_node = NULL;
    _client_node_s *client_node = NULL;

    do {
        list_node = HY_MEM_CALLOC_BREAK(HyPackageListNode_s*, sizeof(*list_node));
        client_node = HY_MEM_CALLOC_BREAK(_client_node_s *, sizeof(*client_node));

        list_node->user_data = client_node;

        return list_node;
    } while(0);

    return NULL;
}

static void _event_base_destroy_cb(void *args)
{
    event_base_free((struct event_base *)args);
}

static void *_event_base_create_cb(void *args)
{
    struct event_base *base = event_base_new();
    if (!base) {
        LOGE("event_base_new failed \n");
    }

    return base;
}

static void _signal_error_cb(void *args)
{
    _main_context_s *context = args;
    context->is_exit = 1;
    event_base_loopexit(context->evlistener_base, NULL);

    LOGE("------error cb\n");
}

static void _signal_user_cb(void *args)
{
    _main_context_s *context = args;
    context->is_exit = 1;
    event_base_loopexit(context->evlistener_base, NULL);

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
    HY_MEMSET(&log_c, sizeof(HyLogConfig_s));
    log_c.port                      = 56789;
    log_c.fifo_len                  = 10 * 1024;
    log_c.config_file               = "../res/hy_log/zlog.conf";
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
        {"thread_pool",     (void **)&context->thread_pool_h,   (HyModuleDestroyHandleCb_t)HyThreadPoolDestroy},
        {"client_plist",    (void **)&context->client_plist_h,  (HyModuleDestroyHandleCb_t)HyPackageListDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_s *context)
{
    HyThreadPoolConfig_s thread_pool_c;
    HY_MEMSET(&thread_pool_c, sizeof(thread_pool_c));
    thread_pool_c.task_item_cnt         = 8;
    thread_pool_c.save_c.run_befor_cb   = _event_base_create_cb;
    thread_pool_c.save_c.run_after_cb   = _event_base_destroy_cb;
    thread_pool_c.save_c.thread_cnt     = 100;

    HyPackageListConfig_s client_plist_c;
    HY_MEMSET(&client_plist_c, sizeof(client_plist_c));
    client_plist_c.save_c.num              = 300 * 4;
    client_plist_c.save_c.node_create_cb   = _client_node_create;
    client_plist_c.save_c.node_destroy_cb  = _client_node_destroy;

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"thread_pool",     (void **)&context->thread_pool_h,   &thread_pool_c,     (HyModuleCreateHandleCb_t)HyThreadPoolCreate,   (HyModuleDestroyHandleCb_t)HyThreadPoolDestroy},
        {"client_plist",    (void **)&context->client_plist_h,  &client_plist_c,    (HyModuleCreateHandleCb_t)HyPackageListCreate,  (HyModuleDestroyHandleCb_t)HyPackageListDestroy},
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

static void _client_read_cb(struct bufferevent *bev, void *args)
{
    char buf[1024];
    hy_s32_t n;

    while (n = bufferevent_read(bev, buf, 1024), n > 0) {
    }
}

static void _client_event_cb(struct bufferevent *bev, short event, void *args)
{
    HyPackageListNode_s *list_node = args;
    _client_node_s *client_node = list_node->user_data;
    _main_context_s *context = client_node->context;

    if (BEV_EVENT_EOF & event) {
        bufferevent_free(bev);
    } else if(BEV_ERROR & event) {
        LOGE("Error occured.\n");
        bufferevent_free(bev);
    }

    for (size_t i = 0; i < HY_UTILS_ARRAY_CNT(ip_arr); i++) {
        if (strcmp(client_node->ip, ip_arr[i]) == 0) {
            HyThreadMutexLock_m(context->client_mutext_h[i]);
            context->client_cnt[i]--;
            HyThreadMutexUnLock_m(context->client_mutext_h[i]);
        }
    }

    for (size_t i = 0; i < 10; i++) {
        if (context->client_cnt[i] > 2) {
            LOGI("check: %d, %d, %d, %d, %d, fd: %d \n",
                 context->client_cnt[0], context->client_cnt[1],
                 context->client_cnt[2], context->client_cnt[3],
                 context->client_cnt[4], client_node->fd);
        }
    }

    HyPackageListTailPut(context->client_plist_h, list_node);
}

static void _task_client_read_cb(void *args, void *run_befor_cb_args)
{
    struct event_base *base = run_befor_cb_args;
    HyPackageListNode_s *list_node = args;
    _client_node_s *client_node = list_node->user_data;
    struct bufferevent *bev;

    bev = bufferevent_socket_new(base, client_node->fd, BEV_OPT_CLOSE_ON_FREE);
    // bufferevent_setwatermark(bev, EV_READ, 32, 0);
    bufferevent_setcb(bev, _client_read_cb, NULL, _client_event_cb, list_node);
    bufferevent_enable(bev, EV_READ);
    bufferevent_disable(bev, EV_WRITE);

    event_base_dispatch(base);
}

static void _listener_cb(struct evconnlistener *listener, evutil_socket_t sock,
                              struct sockaddr *addr, int socklen, void *args)
{
    _main_context_s *context = args;

    HyPackageListNode_s *list_node = NULL;
    _client_node_s *client_node;
    list_node = HyPackageListHeadGet(context->client_plist_h);
    if (!list_node) {
        LOGE("the node is NULL \n");
        return;
    }

    struct sockaddr_in *sock_in = (struct sockaddr_in *)addr;
    char *ip = inet_ntoa(sock_in->sin_addr);

    client_node = list_node->user_data;
    client_node->context = context;
    client_node->fd = sock;
    memset(client_node->ip, 0, 16);
    memcpy(client_node->ip, ip, strlen(ip));

    for (size_t i = 0; i < HY_UTILS_ARRAY_CNT(ip_arr); i++) {
        if (strcmp(client_node->ip, ip_arr[i]) == 0) {
            HyThreadMutexLock_m(context->client_mutext_h[i]);
            context->client_cnt[i]++;
            HyThreadMutexUnLock_m(context->client_mutext_h[i]);
        }
    }

    context->cnt++;
    if (context->cnt % 500 == 0) {
        LOGI("connect: %d, %d, %d, %d, %d, fd: %d \n",
             context->client_cnt[0], context->client_cnt[1],
             context->client_cnt[2], context->client_cnt[3],
             context->client_cnt[4], sock);
    }

    HyThreadPoolTask_s task;
    task.task_cb    = _task_client_read_cb;
    task.args       = list_node;
    HyThreadPoolAddTask(context->thread_pool_h, &task);
}

static void _evconnlistener_destroy(_main_context_s *context)
{
    evconnlistener_free(context->listener);

#ifdef _TEST_1S_EVENT
    event_free(context->time_1s_event);
#endif

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

#ifdef _TEST_1S_EVENT
        context->time_1s_event = event_new(context->evlistener_base, -1,
                                           EV_PERSIST, _time_1s_event_cb, context);
        if (!context->time_1s_event) {
            LOGE("event_new time 1s event failed \n");
            break;
        }
        struct timeval tv_1s = {1, 0};
        evtimer_add(context->time_1s_event, &tv_1s);
#endif

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

        for (size_t i = 0; i < 10; i++) {
            context->client_mutext_h[i] = HyThreadMutexCreate_m();
            if (!context->client_mutext_h[i]) {
                LOGE("HyThreadMutexCreate_m faield \n");
                break;
            }

            context->client_cnt[i] = 0;
        }

        if (0 != _evconnlistener_create(context)) {
            LOGE("_evconnlistener_create failed \n");
            break;
        }

        LOGE("version: %s, data: %s, time: %s \n", VERSION, __DATE__, __TIME__);

        event_base_dispatch(context->evlistener_base);
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
