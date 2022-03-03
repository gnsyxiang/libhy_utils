/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_client.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 16:46
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 16:46
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "ipc_process_client.h"
#include "ipc_link.h"

typedef struct {
    HyIpcProcessSaveConfig_s        save_config;// 必须放在前面，用于强制类型转换

    void                            *ipc_link_h;
} _ipc_process_client_context_s;

void ipc_process_client_destroy(void **ipc_process_client_h)
{
    LOGT("&ipc_process_client_h: %p, ipc_process_client_h: %p \n",
            ipc_process_client_h, *ipc_process_client_h);
    HY_ASSERT_RET(!ipc_process_client_h || !*ipc_process_client_h);

    _ipc_process_client_context_s *context = *ipc_process_client_h;

    ipc_link_destroy(&context->ipc_link_h);

    LOGI("ipc process client destroy, context: %p \n", context);
    HY_MEM_FREE_PP(ipc_process_client_h);
}

void *ipc_process_client_create(HyIpcProcessConfig_s *ipc_process_c)
{
    LOGT("ipc_process_c: %p \n", ipc_process_c);
    HY_ASSERT_RET_VAL(!ipc_process_c, NULL);

    _ipc_process_client_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_process_client_context_s *,
                sizeof(*context));

        HY_MEMCPY(&context->save_config,
                &ipc_process_c->save_config, sizeof(context->save_config));

        ipc_link_config_s ipc_link_c;
        HY_MEMSET(&ipc_link_c, sizeof(ipc_link_c));
        ipc_link_c.ipc_name    = ipc_process_c->ipc_name;
        ipc_link_c.tag         = ipc_process_c->tag;
        ipc_link_c.type        = IPC_LINK_TYPE_CLIENT;

        context->ipc_link_h = ipc_link_create(&ipc_link_c);
        if (!context->ipc_link_h) {
            LOGE("ipc link create failed \n");
            break;
        }

        if (0 != ipc_link_connect(context->ipc_link_h, 5)) {
            LOGE("ipc link connect failed \n");
            break;
        }

        LOGI("ipc process client create, context: %p \n", context);
        return  context;
    } while (0);

    LOGE("ipc process client create failed \n");
    ipc_process_client_destroy((void **)&context);
    return NULL;
}

