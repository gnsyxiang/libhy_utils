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

#include <hy_log/hy_log.h>

#include "config.h"

#include "hy_type.h"
#include "hy_mem.h"
#include "hy_string.h"

#include "hy_json.h"

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
    HyJsonFile_s *json_file = NULL;

    json_file = HyJsonFileCreate("../res/hy_utils/demo.json");

    if (json_file->root) {
        _test_json(json_file->root);

        json_file->save_flag = 1;
        HyJsonFileDestroy(&json_file);
    }
}

int main(int argc, char *argv[])
{
    HyLogInit_m(10 * 1024, HY_LOG_LEVEL_TRACE, HY_LOG_OUTFORMAT_ALL);

    LOGE("version: %s, data: %s, time: %s \n", VERSION, __DATE__, __TIME__);

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

    sleep(3);

    HyLogDeInit();

return 0;
}

