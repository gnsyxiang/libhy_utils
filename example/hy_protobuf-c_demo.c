/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_protobuf-c_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    01/12 2023 11:22
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        01/12 2023      create the file
 * 
 *     last modified: 01/12 2023 11:22
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hy_log/hy_log.h>

#include "hy_module.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_signal.h"

#include "address_book.pb-c.h"

#define _APP_NAME "hy_protobuf-c_demo"

typedef struct {
    hy_s32_t    exit_flag;
} _main_context_s;

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_context_s *context = args;
    context->exit_flag = 1;
}

static void _signal_user_cb(void *args)
{
    LOGI("------user cb\n");

    _main_context_s *context = args;
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

static hy_s32_t _do_pack(uint8_t *buf)
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

static hy_s32_t _do_unpack(const uint8_t *buf, size_t len)
{
    Tutorial__Addressbook *address_book = NULL;

    address_book = tutorial__addressbook__unpack(NULL, len, buf);
    if (!address_book) {
        LOGE("user__unpack failed\n");
        return -1;
    }

    for (size_t i = 0; i < address_book->n_people; ++i) {
        Tutorial__Person *people = address_book->people[i];

        LOGI("id: %d \n", people->id);
        LOGI("name: %s \n", people->name);
        LOGI("email: %s \n", people->email);

        LOGI("phone_number: \n");
        for (size_t j = 0; j < people->n_phones; ++j) {
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
    _main_context_s *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_context_s *, sizeof(*context));

        if (0 != _bool_module_create(context)) {
            printf("_bool_module_create failed \n");
            break;
        }

        if (0 != _handle_module_create(context)) {
            LOGE("_handle_module_create failed \n");
            break;
        }

        LOGE("version: %s, date: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

        uint8_t buf[1024] = {0};
        hy_s32_t len = 0;

        len = _do_pack(buf);
        LOGI("len: %d \n", len);

        _do_unpack(buf, len);

        while (!context->exit_flag) {
            sleep(1);
        }

    } while (0);

    _handle_module_destroy(&context);
    _bool_module_destroy();
    HY_MEM_FREE_PP(&context);

    return 0;
}
