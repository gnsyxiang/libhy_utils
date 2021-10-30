/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_cjson_test.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 19:38
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 19:38
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hy_cjson.h"

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#define ALONE_DEBUG 1

typedef struct {
    void *log_handle;
    void *signal_handle;

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
        {"signal",  &context->signal_handle,    HySignalDestroy},
        {"log",     &context->log_handle,       HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context;

    context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_t log_config;
    log_config.save_config.buf_len      = 512;
    log_config.save_config.level        = HY_LOG_LEVEL_TRACE;
    log_config.save_config.color_output = HY_TYPE_FLAG_ENABLE;

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

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",     &context->log_handle,       &log_config,        (create_t)HyLogCreate,      HyLogDestroy},
        {"signal",  &context->signal_handle,    &signal_config,     (create_t)HySignalCreate,   HySignalDestroy},
    };

    RUN_CREATE(module);

    return context;
}

#if (HY_CJSON_USE_TYPE == 1)
static void _test_cjson(cJSON *root)
{
    int height = HyCjsonGetItemInt(-1, root, "image", "thumbnail", "height");
    LOGD("Height: %d \n", height);

    double d = HyCjsonGetItemDouble(5.5, root, "image", "double", 1);
    LOGD("d: %f \n", d);

    const char *buf = HyCjsonGetItemStr("haha", root, "image", "title");
    LOGD("buf: %s \n", buf);
}
#endif

#if (HY_CJSON_USE_TYPE == 2)
static void _test_cjson(cJSON *root)
{
    int height = HyCjsonGetItemInt2(-1, root, "image.thumbnail.height", HY_STRLEN("image.thumbnail.height"));
    LOGD("Height: %d \n", height);

    double d = HyCjsonGetItemDouble2(5.5, root, "image.double[1]", HY_STRLEN("image.double[1]"));
    LOGD("d: %f \n", d);

    const char *buf = HyCjsonGetItemStr2("haha", root, "image.title", HY_STRLEN("image.title"));
    LOGD("buf: %s \n", buf);
}
#endif

static void _test_cjson_file(void)
{
    cJSON *root = HyCjsonFileParseCreate("../res/hy_utils/cjson.rc");

    _test_cjson(root);

    HyCjsonFileParseDestroy(root);
}

int main(int argc, char *argv[])
{
    _main_context_t *context = _module_create();
    if (!context) {
        LOGE("_module_create faild \n");
        return -1;
    }

    LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    char text_json[]="{\n\
        \"image\": {\n\
            \"width\":  800,\n\
            \"height\": 600,\n\
            \"title\":  \"View from 15th Floor\",\n\
            \"thumbnail\": {\n\
                \"url\":    \"http:/*www.example.com/image/481989943\",\n\
                \"height\": 125,\n\
                \"width\":  100\n\
            },\n\
            \"int\": [1, 2, 3, 4],\n\
            \"double\": [1.1, 0.2, 0.3],\n\
            \"string\": [\"haha\", \"heihei\"]\n\
        }\n\
    }";

    cJSON *root = cJSON_Parse(text_json);
    _test_cjson(root);
    cJSON_Delete(root);

    _test_cjson_file();

    while (!context->exit_flag) {
        sleep(1);
    }

    _module_destroy(&context);

return 0;
}

