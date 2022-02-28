/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_client.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    24/02 2022 16:01
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        24/02 2022      create the file
 * 
 *     last modified: 24/02 2022 16:01
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_string.h"

#include "ipc_link.h"
#include "ipc_process_client.h"

typedef struct {
    HyIpcProcessSaveConfig_s    save_config;

    pid_t                       pid;
    ipc_link_s                  *ipc_link;
    void                        *handle_msg_thread_h;
    hy_s32_t                    exit_flag;
} _ipc_process_client_context_s;

hy_s32_t ipc_process_client_write_sync(void *handle, void *msg, hy_u32_t len)
{
    LOGT("handle: %p, msg: %p, len: %d \n", handle, msg, len);
    HY_ASSERT(handle);
    HY_ASSERT(msg);

    hy_s32_t offset = 0;
    hy_u32_t total_len = 0;
    char *ipc_msg_buf = NULL;
    ipc_link_msg_s *ipc_msg = NULL;
    _ipc_process_client_context_s *context = handle;

    total_len = HY_MEM_ALIGN4_UP(sizeof(ipc_link_msg_s) + len);

    ipc_msg_buf = HY_MEM_MALLOC_RET_VAL(char *, total_len, -1);
    ipc_msg = (ipc_link_msg_s *)ipc_msg_buf;

    HY_MEMCPY(ipc_msg->buf + offset, msg, len);
    offset += len;

    ipc_msg->total_len  = sizeof(ipc_link_msg_s) + offset;
    ipc_msg->type       = IPC_LINK_MSG_TYPE_CB;
    ipc_msg->thread_id  = pthread_self();
    ipc_msg->buf_len    = offset;

    return ipc_link_write(context->ipc_link, ipc_msg);
}

static void _process_client_parse_info_cb(HyIpcProcessInfo_s *ipc_process_info,
        HyIpcProcessConnectState_e is_connect, void *args)
{
    LOGT("ipc_process_info: %p, is_connect: %d, args: %p \n",
            ipc_process_info, is_connect, args);
    HY_ASSERT_RET(!ipc_process_info || !args);

    _ipc_process_client_context_s *context = args;
    HyIpcProcessSaveConfig_s *save_config = &context->save_config;

    if (save_config->connect_change) {
        save_config->connect_change(ipc_process_info,
                is_connect, save_config->args);
    }
}

static hy_s32_t _process_client_handle_msg_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ipc_process_client_context_s *context = args;
    HyIpcProcessSaveConfig_s *save_config = &context->save_config;
    fd_set read_fs = {0};
    struct timeval timeout = {0};
    hy_s32_t ret = 0;
    hy_s32_t fd = 0;
    ipc_link_parse_msg_cb_s parse_msg_cb;

    LOGI("ipc process client handle msg start \n");

    parse_msg_cb.parse_info_cb = _process_client_parse_info_cb;
    parse_msg_cb.args = context;

    fd = ipc_link_get_fd(context->ipc_link);

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);
        FD_SET(fd, &read_fs);

        timeout.tv_sec = 1;
        ret = select(FD_SETSIZE, &read_fs, NULL, NULL, &timeout);
        if (ret < 0) {
            LOGES("select failed \n");
            break;
        }

        if (FD_ISSET(fd, &read_fs)) {
            if (-1 == ipc_link_parse_msg(context->ipc_link, &parse_msg_cb)) {
                LOGE("ipc link parse msg failed \n");

                if (save_config->connect_change) {
                    save_config->connect_change(NULL,
                            HY_IPC_PROCESS_STATE_DISCONNECT, save_config->args);
                }
                break;
            }
        }
    }

    LOGI("ipc process client handle msg stop \n");

    return -1;
}

void ipc_process_client_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_process_client_context_s *context = *handle;

    context->exit_flag = 1;
    HyThreadDestroy(&context->handle_msg_thread_h);

    ipc_link_destroy(&context->ipc_link);

    LOGI("ipc process client destroy, context: %p \n", context);
    HY_MEM_FREE_PP(handle);
}

void *ipc_process_client_create(HyIpcProcessConfig_s *config)
{
    LOGT("config: %p \n", config);
    HY_ASSERT_RET_VAL(!config, NULL);

    _ipc_process_client_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_process_client_context_s *,
                sizeof(*context));

        context->pid = getpid();

        HyIpcProcessSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        context->ipc_link = ipc_link_create(config->ipc_name,
                config->tag, IPC_LINK_TYPE_CLIENT, NULL);
        if (!context->ipc_link) {
            LOGE("ipc link create failed \n");
            break;
        }

        if (0 != ipc_link_connect(context->ipc_link, config->timeout_s)) {
            LOGE("ipc link connect failed \n");
            break;
        }

        if (0 != ipc_link_write_info(context->ipc_link,
                    context->ipc_link->tag, context->pid)) {
            LOGE("ipc link write info failed \n");
            break;
        }

        context->handle_msg_thread_h = HyThreadCreate_m("hy_c_handle_msg",
                _process_client_handle_msg_cb, context);
        if (!context->handle_msg_thread_h) {
            LOGE("hy thread create failed \n");
            break;
        }

        LOGI("ipc process client create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc process client failed \n");
    ipc_process_client_destroy((void **)&context);
    return NULL;
}
