/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_package_list_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/04 2023 15:29
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/04 2023      create the file
 * 
 *     last modified: 21/04 2023 15:29
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "hy_type.h"
#include "hy_mem.h"
#include "hy_assert.h"
#include "hy_string.h"
#include "hy_signal.h"
#include "hy_module.h"
#include "hy_utils.h"
#include "hy_thread.h"
#include "hy_thread_mutex.h"
#include "hy_package_list.h"

#define _APP_NAME "hy_package_list_demo"

typedef struct {
    hy_s32_t                is_exit;
    HyPackageList_s         *package_list_h;

    HyThread_s              *get_h;
    HyThread_s              *put_h;

    HyThreadMutex_s         *mutex_h;
    struct hy_list_head     list;
} _main_context_s;

typedef struct {
    hy_s32_t    num;
    char        *buf;
} _client_data_s;

static void _package_list_node_destroy_cb(HyPackageListNode_s **node_pp)
{
    HY_ASSERT_RET(!node_pp || !*node_pp);
    HyPackageListNode_s *node = *node_pp;
    _client_data_s *client_data = node->user_data;

    if (client_data) {
        if (client_data->buf) {
            HY_MEM_FREE_PP(&client_data->buf);
        }

        HY_MEM_FREE_PP(&client_data);
    }

    HY_MEM_FREE_PP(node_pp);
}

static HyPackageListNode_s *_package_list_node_create_cb(void)
{
    HyPackageListNode_s *node = NULL;
    _client_data_s *client_data = NULL;

    do {
        node = HY_MEM_CALLOC_BREAK(HyPackageListNode_s *, sizeof(*node));
        client_data = HY_MEM_CALLOC_BREAK(_client_data_s *, sizeof(*client_data));
        client_data->buf = HY_MEM_CALLOC_BREAK(char *, 1024);

        node->user_data = client_data;

        LOGI("node: %p, client_data: %p \n", node, client_data);
        return node;
    } while (0);

    _package_list_node_destroy_cb(&node);
    return NULL;
}

static hy_s32_t _get_loop_cb(void *args)
{
    _main_context_s *context = args;
    HyPackageListNode_s *node;
    _client_data_s *client_data;
    hy_s32_t cnt = 0;

    while (!context->is_exit) {
        node = HyPackageListHeadGet(context->package_list_h);
        if (!node) {
            LOGE("HyPackageListGet failed \n");

            usleep(200 * 1000);
            continue;
        }
        client_data = node->user_data;

        snprintf(client_data->buf, 1024, "cnt: %d", cnt);
        client_data->num = cnt++;

        HyThreadMutexLock_m(context->mutex_h);
        hy_list_add_tail(&node->entry, &context->list);
        HyThreadMutexUnLock_m(context->mutex_h);

        usleep(200 * 1000);
    }

    return -1;
}

static hy_s32_t _put_loop_cb(void *args)
{
    _main_context_s *context = args;
    HyPackageListNode_s *pos, *n;
    _client_data_s *client_data;

    while (!context->is_exit) {
        HyThreadMutexLock_m(context->mutex_h);
        hy_list_for_each_entry_safe(pos, n, &context->list, entry) {
            hy_list_del(&pos->entry);
            HyThreadMutexUnLock_m(context->mutex_h);

            client_data = pos->user_data;
            LOGI("buf: %s, cnt: %d \n", client_data->buf, client_data->num);

            HyPackageListTailPut(context->package_list_h, pos);

            usleep(500 * 1000);

            HyThreadMutexLock_m(context->mutex_h);
        }
        HyThreadMutexUnLock_m(context->mutex_h);
    }

    return -1;
}

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_context_s *context = args;
    context->is_exit = 1;
}

static void _signal_user_cb(void *args)
{
    LOGW("------user cb\n");

    _main_context_s *context = args;
    context->is_exit = 1;
}

static void _bool_module_destroy(void)
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
    log_c.save_c.level              = HY_LOG_LEVEL_TRACE;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL;

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

static void _handle_module_destroy(_main_context_s *context)
{
    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"get_thread",          (void **)&context->get_h,                      (HyModuleDestroyHandleCb_t)HyThreadDestroy},
        {"put_thread",          (void **)&context->put_h,                      (HyModuleDestroyHandleCb_t)HyThreadDestroy},
        {"package_list",        (void **)&context->package_list_h,             (HyModuleDestroyHandleCb_t)HyPackageListDestroy},
        {"mutex",               (void **)&context->mutex_h,                    (HyModuleDestroyHandleCb_t)HyThreadMutexDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_s *context)
{
    HyThreadMutexConfig_s mutex_c;
    HY_MEMSET(&mutex_c, sizeof(mutex_c));

    HyThreadConfig_s thread_get_c;
    HY_MEMSET(&thread_get_c, sizeof(thread_get_c));
    thread_get_c.save_c.thread_loop_cb    = _get_loop_cb;
    thread_get_c.save_c.args              = context;
    strcpy(thread_get_c.save_c.name, "get");

    HyThreadConfig_s thread_put_c;
    HY_MEMSET(&thread_put_c, sizeof(thread_put_c));
    thread_put_c.save_c.thread_loop_cb    = _put_loop_cb;
    thread_put_c.save_c.args              = context;
    strcpy(thread_put_c.save_c.name, "put");

    HyPackageListConfig_s package_list_c;
    HY_MEMSET(&package_list_c, sizeof(package_list_c));
    package_list_c.save_c.num = 8;
    package_list_c.save_c.create_cb = _package_list_node_create_cb;
    package_list_c.save_c.destroy_cb = _package_list_node_destroy_cb;

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"mutex",               (void **)&context->mutex_h,            &mutex_c,            (HyModuleCreateHandleCb_t)HyThreadMutexCreate,     (HyModuleDestroyHandleCb_t)HyThreadMutexDestroy},
        {"package_list",        (void **)&context->package_list_h,     &package_list_c,     (HyModuleCreateHandleCb_t)HyPackageListCreate,     (HyModuleDestroyHandleCb_t)HyPackageListDestroy},
        {"get_thread",          (void **)&context->get_h,              &thread_get_c,       (HyModuleCreateHandleCb_t)HyThreadCreate,          (HyModuleDestroyHandleCb_t)HyThreadDestroy},
        {"put_thread",          (void **)&context->put_h,              &thread_put_c,       (HyModuleCreateHandleCb_t)HyThreadCreate,          (HyModuleDestroyHandleCb_t)HyThreadDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

int main(int argc, char *argv[])
{
    _main_context_s *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_s *, sizeof(*context));

        HY_INIT_LIST_HEAD(&context->list);

        if (0 != _bool_module_create(context)) {
            printf("_bool_module_create failed \n");
            break;
        }

        if (0 != _handle_module_create(context)) {
            LOGE("_handle_module_create failed \n");
            break;
        }

        LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

        hy_u32_t cnt = 0;
        while (!context->is_exit) {
            cnt = HyPackageListGetNodeCount(context->package_list_h);
            LOGI("cnt: %d \n", cnt);

            sleep(1);
        }
    } while (0);

    _handle_module_destroy(context);
    _bool_module_destroy();
    HY_MEM_FREE_PP(&context);

    return 0;
}

