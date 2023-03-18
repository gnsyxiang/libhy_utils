/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    HyThreadSpecificData.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    28/04 2022 10:47
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        28/04 2022      create the file
 * 
 *     last modified: 28/04 2022 10:47
 */
#include <stdio.h>

#include "hy_thread_specific_data.h"
#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_log.h"

static HyThreadSpecificData_s context;

static hy_s32_t _thread_specific_data_set(void *handle)
{
    if (!handle) {
        LOGE("the param is error \n");
        return -1;
    }

    if (0 != pthread_setspecific(context.thread_key, handle)) {
        LOGE("pthread_setspecific fail \n");
        return -1;
    } else {
        return 0;
    }
}

static void *_thread_specific_data_get(void)
{
    void *handle = NULL;

    handle = pthread_getspecific(context.thread_key);
    if (!handle) {
        LOGI("pthread_getspecific failed \n");
        return NULL;
    } else {
        return handle;
    }
}

static void _thread_specific_data_destroy(void *args)
{
    if (context.destroy_cb) {
        context.destroy_cb(args);
    }
}

static void _main_thread_specific_data_destroy(void)
{
    _thread_specific_data_destroy(_thread_specific_data_get());
}

void *HyThreadSpecificDataFetch(void)
{
    void *handle = _thread_specific_data_get();
    if (!handle) {
        if (context.destroy_cb) {
            handle = context.create_cb();
            if (!handle) {
                LOGE("context.create_cb failed \n");
                return NULL;
            }
        }

        _thread_specific_data_set(handle);
    } else {
        if (context.reset_cb) {
            context.reset_cb(handle);
        }
    }

    return handle;
}

void HyThreadSpecificDataDestroy(void)
{
    if (0 != atexit(_main_thread_specific_data_destroy)) {
        LOGE("atexit fail \n");
    }
}

hy_s32_t HyThreadSpecificDataCreate(
    HyThreadSpecificDataCreateCb_t create_cb,
    HyThreadSpecificDataDestroyCb_t destroy_cb,
    HyThreadSpecificDataResetCb_t reset_cb)
{
    HY_ASSERT_RET_VAL(!create_cb, -1);
    HY_ASSERT_RET_VAL(!destroy_cb, -1);

    do {
        memset(&context, '\0', sizeof(context));

        context.create_cb  = create_cb;
        context.destroy_cb = destroy_cb;
        context.reset_cb   = reset_cb;

        if (0 != pthread_key_create(&context.thread_key,
                                    _thread_specific_data_destroy)) {
            LOGE("pthread_key_create failed \n");
            break;
        }

        return 0;
    } while (0);

    return -1;
}

