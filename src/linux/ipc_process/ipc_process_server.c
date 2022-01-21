/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_server.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 16:47
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 16:47
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"

#include "ipc_process_server.h"
#include "ipc_process_link_manager.h"

void ipc_process_server_destroy(ipc_process_context_s **context_pp)
{
    LOGT("&context: %p, context: %p \n", context_pp, *context_pp);
    HY_ASSERT_RET(!context_pp || !*context_pp);

    ipc_process_context_s *context = *context_pp;

    ipc_process_link_manager_destroy((ipc_process_link_manager_s **)&context->link_manager);
}

hy_s32_t ipc_process_server_create(ipc_process_context_s *context,
        const char *ipc_name)
{
    do {
        context->link_manager = ipc_process_link_manager_create(ipc_name);
        if (!context->link_manager) {
            LOGE("ipc_process_link_manager_create failed \n");
            break;
        }

        return 0;
    } while (0);

    return -1;
}
