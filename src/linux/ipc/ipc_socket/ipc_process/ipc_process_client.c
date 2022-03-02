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

    HyIpcProcessCallbackCb_s    *callback;
    hy_u32_t                    callback_cnt;

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

    total_len = sizeof(ipc_link_msg_s) + len;

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

static hy_s32_t _ipc_client_parse_msg(ipc_link_s *ipc_link,
        _ipc_process_client_context_s *context)
{
    pid_t pid = 0;
    HyIpcProcessMsgId_e msg_id = 0;
    ipc_link_msg_s *ipc_msg = NULL;
    HyIpcProcessInfo_s ipc_process_info;
    HyIpcProcessSaveConfig_s *save_config = &context->save_config;

    if (0 != ipc_link_read(ipc_link, &ipc_msg)) {
        LOGE("ipc link read failed \n");
        return -1;
    }

    LOGD("ipc msg type: %d \n", ipc_msg->type);
    switch (ipc_msg->type) {
        case IPC_LINK_MSG_TYPE_ACK:
            LOGE("--------haha----return \n");
            break;
        case IPC_LINK_MSG_TYPE_CB:
            msg_id = *(HyIpcProcessMsgId_e *)ipc_msg->buf;

            for (hy_u32_t i = 0; i < context->callback_cnt; ++i) {
                if (context->callback[i].id == msg_id) {
                    context->callback[i].callback_cb(ipc_msg->buf,
                            ipc_msg->buf_len, context->callback[i].args);

                    break;
                }
            }
            break;
        case IPC_LINK_MSG_TYPE_INFO:
            pid = *(pid_t *)(ipc_msg->buf + HY_STRLEN(ipc_msg->buf) + 1);
            ipc_link_set_info(ipc_link, ipc_msg->buf, pid);

            ipc_process_info.tag        = ipc_link->tag;
            ipc_process_info.pid        = ipc_link->pid;
            ipc_process_info.ipc_name   = HyIpcSocketGetName(ipc_link->ipc_socket_h);

            if (save_config->connect_change) {
                save_config->connect_change(&ipc_process_info,
                        HY_IPC_PROCESS_STATE_CONNECT, save_config->args);
            }

            if (ipc_msg) {
                HY_MEM_FREE_PP(&ipc_msg);
            }
            break;
        default:
            LOGE("error ipc_msg type\n");
            break;
    }

    return 0;
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

    LOGI("ipc process client handle msg start \n");

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
            if (-1 == _ipc_client_parse_msg(context->ipc_link, context)) {
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

    if (context->callback) {
        HY_MEM_FREE_PP(&context->callback);
    }
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

        HY_MEMCPY(&context->save_config,
                &config->save_config, sizeof(context->save_config));

        if (config->callback_cnt) {
            context->callback_cnt = config->callback_cnt;

            context->callback = HY_MEM_MALLOC_BREAK(HyIpcProcessCallbackCb_s *,
                    sizeof(HyIpcProcessCallbackCb_s) * config->callback_cnt);

            HY_MEMCPY(context->callback, config->callback,
                    sizeof(HyIpcProcessCallbackCb_s) * config->callback_cnt);
        }

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

        hy_u32_t id[config->callback_cnt];
        for (hy_u32_t i = 0; i < config->callback_cnt; ++i) {
            id[i] = config->callback[i].id;
        }
        ipc_link_write_cb(context->ipc_link, id, config->callback_cnt);

        LOGI("ipc process client create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc process client failed \n");
    ipc_process_client_destroy((void **)&context);
    return NULL;
}
