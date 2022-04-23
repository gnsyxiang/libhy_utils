/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_json_demo.c
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

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_signal.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#include "hy_json.h"

#define _APP_NAME "hy_json_demo"

typedef struct {
    void        *signal_h;

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

static void _module_destroy(_main_context_t **context_pp)
{
    _main_context_t *context = *context_pp;

    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"signal",  &context->signal_h,    HySignalDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context;

    context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);


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
    signal_c.save_c.app_name      = _APP_NAME;
    signal_c.save_c.coredump_path = "./";
    signal_c.save_c.error_cb      = _signal_error_cb;
    signal_c.save_c.user_cb       = _signal_user_cb;
    signal_c.save_c.args          = context;

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"signal",  &context->signal_h,    &signal_c,     (HyModuleCreateHandleCb_t)HySignalCreate,   HySignalDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);

    return context;
}

#if (HY_JSON_USE_TYPE == 1)
static void _test_json(void *root)
{
    hy_s32_t height;
    double d;
    const char *buf;

    height = HyJsonGetItemInt(-1, root, "image", "thumbnail", "height");
    LOGD("image.thumbnail.height: %d \n", height);
    HyJsonSetItemInt(400, root, "image", "thumbnail", "height");
    height = HyJsonGetItemInt(-1, root, "image", "thumbnail", "height");
    LOGD("image.thumbnail.height: %d \n", height);

    d = HyJsonGetItemReal(5.5, root, "image", "double", 1);
    LOGD("image.double: %f \n", d);
    HyJsonSetItemReal(6.6, root, "image", "double", 1);
    d = HyJsonGetItemReal(5.5, root, "image", "double", 1);
    LOGD("image.double: %f \n", d);

    buf = HyJsonGetItemStr("haha", root, "image", "title");
    LOGD("image.title: %s \n", buf);
    HyJsonSetItemStr("hello world", root, "image", "title");
    buf = HyJsonGetItemStr("haha", root, "image", "title");
    LOGD("image.title: %s \n", buf);
}
#endif

#if (HY_JSON_USE_TYPE == 2)
static void _test_json(void *root)
{
    hy_s32_t height = HyJsonGetItemInt2(-1, root, "image.thumbnail.height", HY_STRLEN("image.thumbnail.height"));
    LOGD("Height: %d \n", height);

    double d = HyJsonGetItemReal2(5.5, root, "image.double[1]", HY_STRLEN("image.double[1]"));
    LOGD("d: %f \n", d);

    const char *buf = HyJsonGetItemStr2("haha", root, "image.title", HY_STRLEN("image.title"));
    LOGD("buf: %s \n", buf);
}
#endif

static void _test_json_file(void)
{
    void *root = HyJsonFileCreate("../res/hy_utils/demo.json");

    if (root) {
        _test_json(root);

        HyJsonFileDestroy(root);
    }
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
                \"url\":    \"http://www.example.com/image/481989943\",\n\
                \"height\": 125,\n\
                \"width\":  100\n\
            },\n\
            \"int\": [1, 2, 3, 4],\n\
            \"double\": [1.1, 0.2, 0.3],\n\
            \"string\": [\"haha\", \"heihei\"]\n\
        }\n\
    }";

    void *root = HyJsonCreate(text_json);
    _test_json(root);
    HyJsonDestroy(root);

    _test_json_file();

    while (!context->exit_flag) {
        sleep(1);
    }

    _module_destroy(&context);

return 0;
}

