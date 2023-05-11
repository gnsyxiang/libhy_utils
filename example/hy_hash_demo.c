/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_hash_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 17:20
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 17:20
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "hy_type.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_signal.h"
#include "hy_module.h"
#include "hy_utils.h"

#include "hy_hash.h"

#define _APP_NAME "hy_hash_demo"

typedef struct {
    char name[32];

    hy_s32_t score;
    hy_u32_t age;
} _student_t;

typedef struct {
    void        *hash_handle;

    hy_s32_t    exit_flag;
} _main_context_t;

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_context_t *context = args;
    context->exit_flag = 1;
}

static void _signal_user_cb(void *args)
{
    LOGI("------user cb\n");

    _main_context_t *context = args;
    context->exit_flag = 1;
}

static void _bool_module_destroy(void)
{
    HyModuleDestroyBool_s bool_module[] = {
        {"signal",          HySignalDestroy },
        {"log",             HyLogDeInit     },
    };

    HY_MODULE_RUN_DESTROY_BOOL(bool_module);
}

static hy_s32_t _bool_module_create(_main_context_t *context)
{
    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(log_c));
    log_c.config_file               = "../res/hy_log/zlog.conf";
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.mode               = HY_LOG_MODE_PROCESS_SINGLE;
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
    memset(&signal_c, 0, sizeof(signal_c));
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

static void _handle_module_destroy(_main_context_t *context)
{
    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"hash",    &context->hash_handle,      HyHashDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_t *context)
{
    HyHashConfig_t hash_config;
    hash_config.save_config.bucket_cnt = 32;

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"hash",    &context->hash_handle,      &hash_config,       (HyModuleCreateHandleCb_t)HyHashCreate,     HyHashDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

static void _hash_dump_item_cb(void *val, void *args)
{
    _student_t *st = val;
    LOGD("name: %s, age: %d, score: %d \n", st->name, st->age, st->score);
}

int main(int argc, char *argv[])
{
    _main_context_t *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_t *, sizeof(*context));

        if (0 != _bool_module_create(context)) {
            printf("_bool_module_create failed \n");
            break;
        }

        if (0 != _handle_module_create(context)) {
            LOGE("_handle_module_create failed \n");
            break;
        }

        LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

        {
            HyHashItem_t h_item;
            _student_t *st = malloc(sizeof(*st));

            for (int i = 0; i < 4; ++i) {
                HY_MEMSET(st, sizeof(*st));
                snprintf(st->name, 32, "jim%d", i);
                st->age = 10 + i;
                st->score = 80 + i;

                h_item.key      = st->name;
                h_item.val      = st;
                h_item.val_len  = sizeof(*st);

                HyHashItemAdd(context->hash_handle, &h_item);
            }

            free(st);

            HyHashDump(context->hash_handle, _hash_dump_item_cb, context);
        }

        {
            HyHashItem_t h_item;
            _student_t *st = malloc(sizeof(*st));

            for (int i = 0; i < 4; ++i) {
                HY_MEMSET(st, sizeof(*st));
                snprintf(st->name, 32, "jim%d", i);

                h_item.key      = st->name;
                h_item.val      = st;

                HyHashItemGet(context->hash_handle, &h_item);

                LOGD("name: %s, age: %d, score: %d \n", st->name, st->age, st->score);
            }

            free(st);
        }

        while (!context->exit_flag) {
            sleep(1);
        }

    } while (0);

    _handle_module_destroy(context);
    _bool_module_destroy();
    HY_MEM_FREE_PP(&context);

    return 0;
}

