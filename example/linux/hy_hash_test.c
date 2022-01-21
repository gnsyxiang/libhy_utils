/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_hash_test.c
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

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#include "hy_hash.h"

typedef struct {
    char name[32];

    hy_s32_t score;
    hy_u32_t age;
} _student_t;

typedef struct {
    void *log_handle;
    void *signal_handle;
    void *hash_handle;

    hy_s32_t exit_flag;
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

static void _module_destroy(_main_context_t **context_pp)
{
    _main_context_t *context = *context_pp;

    // note: 增加或删除要同步到module_create_t中
    module_destroy_t module[] = {
        {"hash",    &context->hash_handle,      HyHashDestroy},
        {"signal",  &context->signal_handle,    HySignalDestroy},
        {"log",     &context->log_handle,       HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_s log_config;
    log_config.save_config.buf_len_min  = 512;
    log_config.save_config.buf_len_max  = 512;
    log_config.save_config.level        = HY_LOG_LEVEL_TRACE;
    log_config.save_config.color_enable = HY_TYPE_FLAG_ENABLE;

    int8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    int8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGINT, SIGTERM, SIGUSR1, SIGUSR2,
    };

    HySignalConfig_t signal_config;
    memset(&signal_config, 0, sizeof(signal_config));
    HY_MEMCPY(signal_config.error_num, signal_error_num, sizeof(signal_error_num));
    HY_MEMCPY(signal_config.user_num, signal_user_num, sizeof(signal_user_num));
    signal_config.save_config.app_name      = "template";
    signal_config.save_config.coredump_path = "./";
    signal_config.save_config.error_cb      = _signal_error_cb;
    signal_config.save_config.user_cb       = _signal_user_cb;
    signal_config.save_config.args          = context;

    HyHashConfig_t hash_config;
    hash_config.save_config.bucket_cnt = 32;

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",     &context->log_handle,       &log_config,        (create_t)HyLogCreate,      HyLogDestroy},
        {"signal",  &context->signal_handle,    &signal_config,     (create_t)HySignalCreate,   HySignalDestroy},
        {"hash",    &context->hash_handle,      &hash_config,       (create_t)HyHashCreate,     HyHashDestroy},
    };

    RUN_CREATE(module);

    return context;
}

static void _hash_dump_item_cb(void *val, void *args)
{
    _student_t *st = val;
    LOGD("name: %s, age: %d, score: %d \n", st->name, st->age, st->score);
}

int main(int argc, char *argv[])
{
    _main_context_t *context = _module_create();
    if (!context) {
        LOGE("_module_create faild \n");
        return -1;
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

    _module_destroy(&context);

    return 0;
}

