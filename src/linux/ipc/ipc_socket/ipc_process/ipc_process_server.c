/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_server.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    24/02 2022 15:41
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        24/02 2022      create the file
 * 
 *     last modified: 24/02 2022 15:41
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_string.h"

#include "ipc_link_server.h"
#include "ipc_process_server.h"

typedef struct {
    HyIpcProcessSaveConfig_s    save_config;

    void                        *ipc_link_handle;

    hy_s32_t                    pfd[2];
    void                        *msg_thread_handle;
    hy_s32_t                    exit_flag;
} _ipc_process_server_context_s;

static void _ipc_process_accept_cb(void *handle, void *args)
{
    LOGT("handle: %p, args: %p \n", handle, args);
    HY_ASSERT_RET(!handle);

    _ipc_process_server_context_s *context = args;

    LOGI("ipc_process_server accept new client: %p \n", handle);

    write(context->pfd[1], &handle, sizeof(void *));
}

static hy_s32_t _ipc_process_detect_cb(void *args)
{
    return 0;
}

static hy_s32_t _ipc_process_msg_handle_cb(void *args)
{
    _ipc_process_server_context_s *context = args;
    fd_set read_fs = {0};
    struct timeval timeout = {0};
    hy_s32_t ret = 0;
    void *client_link_handle = NULL;

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);
        FD_SET(context->pfd[0], &read_fs);

        ipc_link_server_set_fd(context->ipc_link_handle, &read_fs);

        timeout.tv_sec = 5;
        ret = select(FD_SETSIZE, &read_fs, NULL, NULL, &timeout);
        if (ret < 0) {
            LOGES("select failed \n");
            break;
        }

        if (FD_ISSET(context->pfd[0], &read_fs)) {
            read(context->pfd[0], &client_link_handle, sizeof(void *));
            LOGE("---haha, client_link_handle: %p \n", client_link_handle);
        }

        ipc_link_server_detect_fd(context->ipc_link_handle, &read_fs,
                _ipc_process_detect_cb, context);
    }

    return -1;
}

void ipc_process_server_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_process_server_context_s *context = *handle;

    context->exit_flag = 1;
    HyThreadDestroy(&context->msg_thread_handle);

    ipc_link_server_destroy(&context->ipc_link_handle);

    LOGI("ipc process server destroy, context: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *ipc_process_server_create(HyIpcProcessConfig_s *config)
{
    LOGT("config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    _ipc_process_server_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_process_server_context_s *,
                sizeof(*context));

        pipe(context->pfd);

        HyIpcProcessSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        context->ipc_link_handle = ipc_link_server_create(config->ipc_name,
                config->tag, _ipc_process_accept_cb, context);
        if (!context->ipc_link_handle) {
            LOGE("ipc_link_server_create failed \n");
            break;
        }

        context->msg_thread_handle = HyThreadCreate_m("hy_s_handle_msg",
                _ipc_process_msg_handle_cb, context);
        if (!context->msg_thread_handle) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        LOGI("ipc process server create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc process server failed \n");
    ipc_process_server_destroy((void **)&context);
    return NULL;
}
