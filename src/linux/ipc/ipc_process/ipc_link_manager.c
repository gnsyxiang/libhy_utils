/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link_manager.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 14:17
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 14:17
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_thread.h"

#include "ipc_link_manager.h"
#include "ipc_link.h"

typedef struct {
    ipc_link_manager_save_config_s  save_config;

    void                            *ipc_link_h;
    void                            *accept_thread_h;
    hy_s32_t                        exit_flag;
} _ipc_link_manager_context_s;

static hy_s32_t _ipc_link_manager_thread_accept_cb(void *args)
{
    LOGT("ipc link manager thread accept cb, args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ipc_link_manager_context_s *context = args;
    ipc_link_manager_save_config_s *save_config = &context->save_config;
    hy_s32_t ret = 0;

    LOGI("ipc link manager thread accept cb start \n");

    ret = ipc_link_wait_accept(context->ipc_link_h,
            save_config->accept_cb, save_config->args);

    LOGI("ipc link manager thread accept cb stop \n");
    return ret;
}

void ipc_link_manager_destroy(void **ipc_link_manager_h)
{
    LOGT("&ipc_link_manager_h: %p, ipc_link_manager_h: %p \n",
            ipc_link_manager_h, *ipc_link_manager_h);
    HY_ASSERT_RET(!ipc_link_manager_h || !*ipc_link_manager_h);

    _ipc_link_manager_context_s *context = *ipc_link_manager_h;

    ipc_link_destroy(&context->ipc_link_h);

    context->exit_flag = 1;
    HyThreadDestroy(&context->accept_thread_h);

    LOGI("ipc link manager destroy, context: %p \n", context);
    HY_MEM_FREE_PP(ipc_link_manager_h);
}

void *ipc_link_manager_create(ipc_link_manager_config_s *ipc_link_manager_c)
{
    LOGT("ipc_link_manager_c: %p \n", ipc_link_manager_c);
    HY_ASSERT_RET_VAL(!ipc_link_manager_c, NULL);

    _ipc_link_manager_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_link_manager_context_s *,
                sizeof(*context));

        HY_MEMCPY(&context->save_config, &ipc_link_manager_c->save_config,
                sizeof(context->save_config));

        context->ipc_link_h = ipc_link_create_m(ipc_link_manager_c->ipc_name,
                ipc_link_manager_c->tag, IPC_LINK_TYPE_SERVER, NULL);
        if (!context->ipc_link_h) {
            LOGE("ipc link create m failed \n");
            break;
        }

        context->accept_thread_h = HyThreadCreate_m("hy_i_l_m_accept",
                _ipc_link_manager_thread_accept_cb, context);
        if (!context->accept_thread_h) {
            LOGE("hy thread create m failed \n");
            break;
        }

        LOGI("ipc link manager create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc link manager failed \n");
    ipc_link_manager_destroy((void **)&context);
    return NULL;
}

