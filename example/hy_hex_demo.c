/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_hex_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    12/04 2022 10:09
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        12/04 2022      create the file
 * 
 *     last modified: 12/04 2022 10:09
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_module.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#include "hy_hex.h"

#define _APP_NAME "hy_hex_demo"

typedef struct {
    void        *log_h;

    hy_s32_t    exit_flag;
} _main_context_t;

static void _module_destroy(_main_context_t **context_pp)
{
    _main_context_t *context = *context_pp;

    // note: 增加或删除要同步到module_create_t中
    module_destroy_t module[] = {
        {"log",         &context->log_h,           HyLogDestroy},
    };

    RUN_DESTROY(module);

    HY_MEM_FREE_PP(context_pp);
}

static _main_context_t *_module_create(void)
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *, sizeof(*context), NULL);

    HyLogConfig_s log_c;
    log_c.save_c.buf_len_min  = 512;
    log_c.save_c.buf_len_max  = 512;
    log_c.save_c.level        = HY_LOG_LEVEL_TRACE;
    log_c.save_c.color_enable = HY_TYPE_FLAG_ENABLE;

    // note: 增加或删除要同步到module_destroy_t中
    module_create_t module[] = {
        {"log",         &context->log_h,           &log_c,        (create_t)HyLogCreate,          HyLogDestroy},
    };

    RUN_CREATE(module);

    return context;
}

int main(int argc, char *argv[])
{
    _main_context_t *context = _module_create();
    if (!context) {
        LOGE("_module_create faild \n");
        return -1;
    }

    LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    char *buf = "1234567890abcdefghi";
    HY_HEX_ASCII(buf, HY_STRLEN(buf));
    HY_HEX(buf, HY_STRLEN(buf));

    sleep(3);

    _module_destroy(&context);

    return 0;
}

