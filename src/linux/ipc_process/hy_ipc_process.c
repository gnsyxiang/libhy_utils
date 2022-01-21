/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_ipc_process.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 11:12
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 11:12
 */
#include <stdio.h>

#include "hy_hal/hy_log.h"
#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_thread.h"

#include "hy_ipc_socket.h"

#include "hy_ipc_process.h"
#include "ipc_process_inside.h"

static void _accept_cb(void *handle, void *args)
{
    LOGD("handle: %p, args: %p \n", handle, args);
}

static hy_s32_t _accept_loop_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    hy_ipc_process_context_s *context = args;

    return HyIpcSocketAccept(context->ipc_socket_handle, _accept_cb, args);
}

void HyIpcProcessDestroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    hy_ipc_process_context_s *context = *handle;

    HyIpcSocketDestroy(&context->ipc_socket_handle);

    HyThreadDestroy(&context->accept_thread_handle);

    HY_MEM_FREE_PP(handle);
}

void *HyIpcProcessCreate(HyIpcProcessConfig_s *config)
{
    LOGT("ipc process config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    hy_ipc_process_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(hy_ipc_process_context_s *,
                sizeof(*context));

        HyIpcProcessSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        context->ipc_socket_handle = HyIpcSocketCreate_m(config->ipc_name,
                HY_IPC_SOCKET_TYPE_SERVER);
        if (!context->ipc_socket_handle) {
            LOGE("HyIpcSocketCreate_m failed \n");
            break;
        }

        context->accept_thread_handle = HyThreadCreate_m("hy_accept",
                _accept_loop_cb, context);
        if (!context->accept_thread_handle) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        LOGI("ipc process create \n");
        return context;
    } while (0);

    LOGI("ipc process create failed \n");
    HyIpcProcessDestroy((void **)&context);
    return NULL;
}
