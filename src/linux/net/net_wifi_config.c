/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    net_wifi_config.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    01/04 2022 15:13
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        01/04 2022      create the file
 * 
 *     last modified: 01/04 2022 15:13
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_barrier.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_file.h"
#include "hy_hal/hy_thread_mutex.h"

#include "net_wifi_config.h"

typedef struct {
    net_wifi_config_save_config_s   save_c;

    void                            *mutex_h;
} _net_wifi_config_context_s;

static hy_s32_t _config_parse_get(_net_wifi_config_context_s *context,
        net_wifi_config_s *net_wifi_c)
{
    HyThreadMutexLock_m(context->mutex_h);
    HyThreadMutexUnLock_m(context->mutex_h);
    return -1;
}

static hy_s32_t _config_parse_set(_net_wifi_config_context_s *context,
        net_wifi_config_s *net_wifi_c)
{
    HyThreadMutexLock_m(context->mutex_h);
    HyThreadMutexUnLock_m(context->mutex_h);
    return -1;
}

hy_s32_t net_wifi_config_load(void *handle, net_wifi_config_s *net_wifi_c)
{
    LOGT("handle: %p, net_wifi_c: %p \n", handle, net_wifi_c);
    HY_ASSERT_RET_VAL(!handle || !net_wifi_c, -1);

    _net_wifi_config_context_s *context = handle;
    net_wifi_config_save_config_s *save_c = &context->save_c;
    hy_s32_t ret = -1;

    if (0 != HyFileIsExist(context->save_c.file_path)) {
        if (save_c->set_default_cb) {
            save_c->set_default_cb(net_wifi_c, save_c->args);

            ret = _config_parse_set(handle, net_wifi_c);
        }
    } else {
        ret = _config_parse_get(handle, net_wifi_c);
    }

    return ret;
}

hy_s32_t net_wifi_config_save(void *handle, net_wifi_config_s *net_wifi_c)
{
    LOGT("handle: %p, net_wifi_c: %p \n", handle, net_wifi_c);
    HY_ASSERT_RET_VAL(!handle || !net_wifi_c, -1);

    return _config_parse_set(handle, net_wifi_c);
}

void net_wifi_config_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _net_wifi_config_context_s *context = *handle;

    HyThreadMutexDestroy(&context->mutex_h);

    HY_MEM_FREE_PP(handle);
}

void *net_wifi_config_create(net_wifi_config_config_s *net_wifi_config_c)
{
    LOGT("net_wifi_config_c: %p \n", net_wifi_config_c);
    HY_ASSERT_RET_VAL(!net_wifi_config_c, NULL);

    _net_wifi_config_context_s *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_net_wifi_config_context_s *,
                sizeof(*context));

        HY_MEMCPY(&context->save_c,
                &net_wifi_config_c->save_c, sizeof(context->save_c));

        context->mutex_h = HyThreadMutexCreate_m();
        if (!context->mutex_h) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        LOGT("net wifi config create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("net wifi config create failed \n");
    net_wifi_config_destroy((void **)&context);
    return NULL;
}

