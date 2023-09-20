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

#include "config.h"

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

    hy_u32_t score;
    hy_u32_t age;
} _student_t;

typedef struct {
    HyHash_s    *hash_h;

    hy_s32_t    is_exit;
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

static void _handle_module_destroy(_main_context_s **context_pp)
{
    _main_context_s *context = *context_pp;

    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"hash",    (void **)&context->hash_h,      (HyModuleDestroyHandleCb_t)HyHashDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_context_s *context)
{
    HyHashConfig_s hash_config;
    hash_config.save_c.bucket_cnt = 32;

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"hash",    (void **)&context->hash_h,      &hash_config,       (HyModuleCreateHandleCb_t)HyHashCreate,     (HyModuleDestroyHandleCb_t)HyHashDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

static void _item_dump_cb(HyHashItem_s *item, void *args)
{
    _student_t *st = item->val;

    LOGI("name: %s, age: %d, score: %d \n", st->name, st->age, st->score);
}

static void _hash_test(_main_context_s *context)
{
    hy_u32_t item_cnt = 4;

    {
        _student_t *st = HY_MEM_CALLOC_RETURN(_student_t *, sizeof(*st));
        HyHashItem_s item;

        for (hy_u32_t i = 0; i < item_cnt; ++i) {
            HY_MEMSET(st, sizeof(*st));

            snprintf(st->name, 32, "jim%d", i);
            st->age = 10 + i;
            st->score = 80 + i;

            item.key      = st->name;
            item.val      = st;
            item.val_len  = sizeof(*st);

            HyHashAdd(context->hash_h, &item);
        }

        HY_MEM_FREE_PP(&st);
    }

    HyHashDumpAll(context->hash_h, _item_dump_cb, context);

    {
        _student_t *st = HY_MEM_CALLOC_RETURN(_student_t *, sizeof(*st));
        char key[32];
        HyHashItem_s item;

        for (hy_u32_t i = 0; i < item_cnt; ++i) {
            snprintf(key, 32, "jim%d", i);

            HY_MEMSET(st, sizeof(*st));
            item.key      = key;
            item.val      = st;
            item.val_len  = sizeof(*st);

            HyHashPeekGet(context->hash_h, &item);

            _item_dump_cb(&item, context);
            LOGD("name: %s, age: %d, score: %d \n", st->name, st->age, st->score);
        }

        HY_MEM_FREE_PP(&st);
    }

    {
        _student_t *st = NULL;
        char key[32];
        HyHashItem_s item;

        for (hy_u32_t i = 0; i < item_cnt; ++i) {
            HY_MEMSET(&item, sizeof(item));

            snprintf(key, 32, "jim%d", i);
            item.key      = key;

            HyHashDel(context->hash_h, &item);

            st = (_student_t *)item.val;

            LOGD("name: %s, age: %d, score: %d \n", st->name, st->age, st->score);

            // NOTE: HyHashDel里面开辟了空间，如果不释放则会内存泄漏
            HY_MEM_FREE_PP(&item.val);
        }
    }

    HyHashDumpAll(context->hash_h, _item_dump_cb, context);
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

        LOGE("version: %s, data: %s, time: %s \n", VERSION, __DATE__, __TIME__);

        _hash_test(context);

        while (!context->is_exit) {
            sleep(1);
        }

    } while (0);

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
