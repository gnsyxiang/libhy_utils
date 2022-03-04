/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_server.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 16:20
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 16:20
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "ipc_process_server.h"
#include "ipc_link_manager.h"
#include "ipc_link.h"

typedef struct {
    HyIpcProcessSaveConfig_s        save_config;// 必须放在前面，用于强制类型转换

    void                            *ipc_link_h;
    void                            *ipc_link_manager_h;
} _ipc_process_server_context_s;

static void _ipc_link_manager_accept_cb(void *ipc_link_h, void *args)
{
    LOGD("ipc_link_h: %p \n", ipc_link_h);
}

void ipc_process_server_destroy(void **ipc_process_server_h)
{
    LOGT("&ipc_process_server_h: %p, ipc_process_server_h: %p \n",
            ipc_process_server_h, *ipc_process_server_h);
    HY_ASSERT_RET(!ipc_process_server_h || !*ipc_process_server_h);

    _ipc_process_server_context_s *context = *ipc_process_server_h;

    ipc_link_destroy(&context->ipc_link_h);

    ipc_link_manager_destroy(&context->ipc_link_manager_h);

    LOGI("ipc process server destroy, context: %p \n", context);
    HY_MEM_FREE_PP(ipc_process_server_h);
}

void *ipc_process_server_create(HyIpcProcessConfig_s *ipc_process_c)
{
    LOGT("ipc_process_c: %p \n", ipc_process_c);
    HY_ASSERT_RET_VAL(!ipc_process_c, NULL);

    _ipc_process_server_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_process_server_context_s *,
                sizeof(*context));

        HY_MEMCPY(&context->save_config,
                &ipc_process_c->save_config, sizeof(context->save_config));

        context->ipc_link_h = ipc_link_create_m(ipc_process_c->ipc_name,
                ipc_process_c->tag, IPC_LINK_TYPE_SERVER, NULL);
        if (!context->ipc_link_h) {
            LOGE("ipc link create m failed \n");
            break;
        }

        ipc_link_manager_config_s ipc_link_manager_c;
        HY_MEMSET(&ipc_link_manager_c, sizeof(ipc_link_manager_c));
        ipc_link_manager_c.save_config.accept_cb    = _ipc_link_manager_accept_cb;
        ipc_link_manager_c.save_config.args         = context;
        ipc_link_manager_c.ipc_link_h               = context->ipc_link_h;
        context->ipc_link_manager_h = ipc_link_manager_create(&ipc_link_manager_c);
        if (!context->ipc_link_manager_h) {
            LOGE("ipc link manager create faeild \n");
            break;
        }

        LOGI("ipc process server create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc process server create failed \n");
    ipc_process_server_destroy((void **)&context);
    return NULL;
}

