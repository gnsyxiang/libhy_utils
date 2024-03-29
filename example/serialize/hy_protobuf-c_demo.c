/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_protobuf-c_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/08 2021 20:51
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/08 2021      create the file
 * 
 *     last modified: 17/08 2021 20:51
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "hy_utils.h"
#include "hy_module.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_signal.h"

#include "address_book.pb-c.h"

#define _APP_NAME "hy_protobuf-c_demo"

typedef struct {
    hy_s32_t    exit_flag;
} _main_context_t;

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

static hy_s32_t _bool_module_create(_main_context_t *context)
{
    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(HyLogConfig_s));
    log_c.port                      = 56789;
    log_c.fifo_len                  = 10 * 1024;
    log_c.config_file               = "../res/hy_log/zlog.conf";
    log_c.save_c.level              = HY_LOG_LEVEL_INFO;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL_NO_PID_ID;

    hy_s8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    hy_s8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
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

static hy_s32_t _do_pack(uhy_s8_t *buf)
{
    Tutorial__Person__PhoneNumber phone_number = TUTORIAL__PERSON__PHONE_NUMBER__INIT;
    Tutorial__Person person = TUTORIAL__PERSON__INIT;
    Tutorial__Addressbook address_book = TUTORIAL__ADDRESSBOOK__INIT;
    Tutorial__Person__PhoneNumber *phone_number_p = &phone_number;
    Tutorial__Person *person_p = &person;

    phone_number.has_type   = 1;
    phone_number.type       = TUTORIAL__PERSON__PHONE_TYPE__HOME;
    phone_number.number     = "110";

    person.id               = 1;
    person.name             = "haha";
    person.email            = "haha@110.com";
    person.n_phones         = 1;
    person.phones           = &phone_number_p;

    address_book.n_people   = 1;
    address_book.people     = &person_p;

    return tutorial__addressbook__pack(&address_book, buf);
}

static hy_s32_t _do_unpack(const uhy_s8_t *buf, hy_u32_t len)
{
    Tutorial__Addressbook *address_book = NULL;

    address_book = tutorial__addressbook__unpack(NULL, len, buf);
    if (!address_book) {
        LOGE("user__unpack failed\n");
        return -1;
    }

    for (hy_u32_t i = 0; i < address_book->n_people; ++i) {
        Tutorial__Person *people = address_book->people[i];

        LOGI("id: %d \n", people->id);
        LOGI("name: %s \n", people->name);
        LOGI("email: %s \n", people->email);

        LOGI("phone_number: \n");
        for (hy_u32_t j = 0; j < people->n_phones; ++j) {
            Tutorial__Person__PhoneNumber *phones = people->phones[j];

            LOGI("\t\ttype: %d \n", phones->type);
            LOGI("\t\tnumber: %s \n", phones->number);
        }
    }

    tutorial__addressbook__free_unpacked(address_book, NULL);

    return 0;
}

int main(int argc, char const* argv[])
{
    _main_context_t *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_t *, sizeof(*context));

        struct {
            const char *name;
            hy_s32_t (*create)(_main_context_s *context);
        } create_arr[] = {
            {"_bool_module_create",     _bool_module_create},
            {"_handle_module_create",   _handle_module_create},
        };
        for (hy_u32_t i = 0; i < HY_UTILS_ARRAY_CNT(create_arr); i++) {
            if (create_arr[i].create) {
                if (0 != create_arr[i].create(context)) {
                    LOGE("%s failed \n", create_arr[i].name);
                }
            }
        }

        LOGE("version: %s, data: %s, time: %s \n", VERSION, __DATE__, __TIME__);

        uhy_s8_t buf[1024] = {0};
        hy_s32_t len = 0;

        len = _do_pack(buf);
        LOGI("len: %d \n", len);

        _do_unpack(buf, len);

        while (!context->exit_flag) {
            sleep(1);
        }

    } while (0);

    void (*destroy_arr[])(_main_context_s **context_pp) = {
        _bool_module_destroy
    };
    for (hy_u32_t i = 0; i < HY_UTILS_ARRAY_CNT(destroy_arr); i++) {
        if (destroy_arr[i]) {
            destroy_arr[i](&context);
        }
    }
    HY_MEM_FREE_PP(&context);

    return 0;
}

