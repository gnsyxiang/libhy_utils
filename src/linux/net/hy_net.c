/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_net.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    02/04 2022 10:30
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        02/04 2022      create the file
 * 
 *     last modified: 02/04 2022 10:30
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_barrier.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_log.h"

#include "hy_net.h"

typedef struct {
    HyNetSaveConfig_s   save_c;
} _net_context_s;

void HyNetDestroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _net_context_s *context = *handle;

    LOGI("net destroy, context: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *HyNetCreate(HyNetConfig_s *net_c)
{
    LOGT("net_c: %p \n", net_c);
    HY_ASSERT_RET_VAL(!net_c, NULL);

    _net_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_net_context_s *, sizeof(*context));

        HY_MEMCPY(&context->save_c, &net_c->save_c, sizeof(context->save_c));

        LOGI("net create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("net create failed \n");
    HyNetDestroy((void **)&context);
    return NULL;
}

