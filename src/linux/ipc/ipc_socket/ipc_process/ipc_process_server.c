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

#include "ipc_link_manager.h"
#include "ipc_process_server.h"

typedef struct {
    HyIpcProcessSaveConfig_s    save_config;

    void                        *ipc_link_manager_h;

    hy_s32_t                    pfd[2];
    void                        *handle_msg_thread_h;
    hy_s32_t                    exit_flag;
} _ipc_process_server_context_s;

static hy_s32_t _process_server_parse_info_cb(void *handle, void *args)
{
    LOGT("handle: %p, args: %p \n", handle, args);
    HY_ASSERT_RET_VAL(!handle || !args, -1);

    _ipc_process_server_context_s *context = args;
    HyIpcProcessInfo_s ipc_process_info;

    ipc_link_get_info(handle, &ipc_process_info);

    HyIpcProcessSaveConfig_s *save_config = &context->save_config;
    if (save_config->connect_change) {
        save_config->connect_change(&ipc_process_info, save_config->args);
    }

    return 0;
}

static void _process_server_link_manager_accept_cb(void *handle, void *args)
{
    LOGT("handle: %p, args: %p \n", handle, args);
    HY_ASSERT_RET(!handle);

    LOGI("ipc process server accept new ipc link: %p \n", handle);

    _ipc_process_server_context_s *context = args;

    write(context->pfd[1], &handle, sizeof(void *));
}

static hy_s32_t _process_server_handle_msg_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ipc_process_server_context_s *context = args;
    fd_set read_fs = {0};
    struct timeval timeout = {0};
    ipc_link_s *pos, *n;
    hy_s32_t fd = 0;
    void *client_link_handle = NULL;
    struct hy_list_head *client_link_list = NULL;
    ipc_link_manager_parse_cb_s parse_cb;

    parse_cb.parse_info_cb = _process_server_parse_info_cb;
    parse_cb.args = context;

    LOGI("ipc process handle msg start \n");

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);
        FD_SET(context->pfd[0], &read_fs);

        client_link_list = ipc_link_manager_get_list(context->ipc_link_manager_h);
        hy_list_for_each_entry(pos, client_link_list, entry) {
            fd = ipc_link_get_fd(pos);
            FD_SET(fd, &read_fs);
        }
        ipc_link_manager_put_list(context->ipc_link_manager_h);

        timeout.tv_sec = 1;
        if (select(FD_SETSIZE, &read_fs, NULL, NULL, &timeout) < 0) {
            LOGES("select failed \n");
            break;
        }

        if (FD_ISSET(context->pfd[0], &read_fs)) {
            LOGI("ipc process server send info to client \n");

            read(context->pfd[0], &client_link_handle, sizeof(void *));

            HyIpcProcessInfo_s ipc_process_info;
            ipc_link_get_info(client_link_handle, &ipc_process_info);

            ipc_link_write_info(client_link_handle, ipc_process_info.pid);
        }

        client_link_list = ipc_link_manager_get_list(context->ipc_link_manager_h);
        hy_list_for_each_entry_safe(pos, n, client_link_list, entry) {
            fd = ipc_link_get_fd(pos);

            if (FD_ISSET(fd, &read_fs)) {
                if (-1 == ipc_link_manager_parse_msg(pos, &parse_cb)) {
                    hy_list_del(&pos->entry);
                    ipc_link_destroy(&pos);

                    continue;
                }
            }
        }
        ipc_link_manager_put_list(context->ipc_link_manager_h);
    }

    LOGI("ipc process handle msg stop \n");

    return -1;
}

void ipc_process_server_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_process_server_context_s *context = *handle;

    context->exit_flag = 1;
    HyThreadDestroy(&context->handle_msg_thread_h);

    ipc_link_manager_destroy(&context->ipc_link_manager_h);

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

        context->ipc_link_manager_h = ipc_link_manager_create(config->ipc_name,
                config->tag, _process_server_link_manager_accept_cb, context);
        if (!context->ipc_link_manager_h) {
            LOGE("ipc link manager create failed \n");
            break;
        }

        context->handle_msg_thread_h= HyThreadCreate_m("hy_s_handle_msg",
                _process_server_handle_msg_cb, context);
        if (!context->handle_msg_thread_h) {
            LOGE("hy thread create failed \n");
            break;
        }

        LOGI("ipc process server create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc process server failed \n");
    ipc_process_server_destroy((void **)&context);
    return NULL;
}
