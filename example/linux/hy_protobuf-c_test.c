/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_protobuf-c_test.c
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

#include "hy_hal/hy_log.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_type.h"

#include "address_book.pb-c.h"

typedef struct {
    void    *log_handle;
    void    *signal_handle;

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
        {"signal",      &context->signal_handle,        HySignalDestroy},
        {"log",         &context->log_handle,           HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_t log_config;
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
    signal_config.save_config.app_name      = "protobuf-c";
    signal_config.save_config.coredump_path = "./";
    signal_config.save_config.error_cb      = _signal_error_cb;
    signal_config.save_config.user_cb       = _signal_user_cb;
    signal_config.save_config.args          = context;

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",         &context->log_handle,           &log_config,            (create_t)HyLogCreate,          HyLogDestroy},
        {"signal",      &context->signal_handle,        &signal_config,         (create_t)HySignalCreate,       HySignalDestroy},
    };

    RUN_CREATE(module);

    return context;
}

static int32_t _do_pack(uint8_t *buf)
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

static int32_t _do_unpack(const uint8_t *buf, size_t len)
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
    _main_context_t *context = _module_create();
    if (!context) {
        LOGE("_module_create faild \n");
        return -1;
    }

    LOGI("version: %s, date: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    uint8_t buf[1024] = {0};
    int32_t len = 0;

    len = _do_pack(buf);
    LOGI("len: %d \n", len);

    _do_unpack(buf, len);

    while (!context->exit_flag) {
        sleep(1);
    }

    _module_destroy(&context);

    return 0;
}
