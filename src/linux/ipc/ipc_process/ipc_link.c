/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 10:46
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 10:46
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "ipc_link.h"
#include "hy_ipc_socket.h"

typedef struct {
    char    tag[IPC_LINK_IPC_NAME_LEN_MAX];
    void    *ipc_socket_h;
} _ipc_link_context_t;

void ipc_link_destroy(void **ipc_link_h)
{
    LOGT("&ipc_link_h: %p, ipc_link_h: %p \n", ipc_link_h, *ipc_link_h);
    HY_ASSERT_RET(!ipc_link_h || !*ipc_link_h);

    _ipc_link_context_t *context = *ipc_link_h;

    HyIpcSocketDestroy(&context->ipc_socket_h);

    LOGI("ipc link destroy, context: %p \n", context);
    HY_MEM_FREE_PP(ipc_link_h);
}

void *ipc_link_create(ipc_link_config_s *config)
{
    LOGT("config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    _ipc_link_context_t *context = NULL;
    HyIpcSocketType_e ipc_socket_type[HY_IPC_SOCKET_TYPE_MAX] = {
        HY_IPC_SOCKET_TYPE_CLIENT, HY_IPC_SOCKET_TYPE_SERVER
    };

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_link_context_t *, sizeof(*context));

        if (config->ipc_socket_h) {
            context->ipc_socket_h = config->ipc_socket_h;
        } else {
            HY_STRNCPY(context->tag, sizeof(context->tag),
                    config->tag, HY_STRLEN(config->tag));

            context->ipc_socket_h = HyIpcSocketCreate_m(config->ipc_name,
                    ipc_socket_type[config->type]);
            if (!context->ipc_socket_h) {
                LOGE("hy ipc socket create failed \n");
                break;
            }
        }

        LOGI("ipc link create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc link create failed \n");
    ipc_link_destroy((void **)&context);
    return NULL;
}

