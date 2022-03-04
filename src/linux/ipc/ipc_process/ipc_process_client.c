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
#include "hy_hal/hy_thread.h"

#include "ipc_process_client.h"
#include "ipc_link.h"

typedef struct {
    HyIpcProcessSaveConfig_s        save_c; // 必须放在前面，用于强制类型转换

    pid_t                           pid;
    hy_u32_t                        connect_timeout_s;
    void                            *ipc_link_h;

    void                            *read_ipc_link_msg_thread_h;
    void                            *connect_thread_h;
    hy_s32_t                        exit_flag:1;
    hy_s32_t                        exit_wait_flag:1;
    HyIpcProcessConnectState_e      is_connect:2;
    hy_s32_t                        reserved;
} _ipc_process_client_context_s;

static void _handle_ipc_link_msg_info(_ipc_process_client_context_s *context,
        ipc_link_msg_s *ipc_msg)
{
    const char *tag = NULL;
    ipc_link_info_s ipc_link_info;
    HyIpcProcessInfo_s ipc_process_info;
    HyIpcProcessSaveConfig_s *save_c = &context->save_c;

    context->pid = *(pid_t *)ipc_msg->buf;
    tag = (const char *)(ipc_msg->buf + sizeof(pid_t));

    ipc_link_info_set(context->ipc_link_h, tag);

    ipc_link_info_get(context->ipc_link_h, &ipc_link_info);

    ipc_process_info.pid        = context->pid;
    ipc_process_info.ipc_name   = ipc_link_info.ipc_name;
    ipc_process_info.tag        = ipc_link_info.tag;

    if (save_c->state_change_cb) {
        save_c->state_change_cb(&ipc_process_info,
                HY_IPC_PROCESS_CONNECT_STATE_CONNECT, save_c->args);
    }
}

static hy_s32_t _handle_ipc_link_msg(_ipc_process_client_context_s *context)
{
    LOGT("context: %p \n", context);
    HY_ASSERT(context);

    ipc_link_msg_s *ipc_msg = NULL;

    if (0 != ipc_link_read(context->ipc_link_h, &ipc_msg)) {
        LOGE("ipc link read failed \n");
        return -1;
    }

    switch (ipc_msg->type) {
        case IPC_LINK_MSG_TYPE_INFO:
            _handle_ipc_link_msg_info(context, ipc_msg);
            break;
        case IPC_LINK_MSG_TYPE_ACK:
            break;
        case IPC_LINK_MSG_TYPE_CB:
            break;
        case IPC_LINK_MSG_TYPE_CB_ID:
            break;
        default:
            break;
    }

    return 0;
}

static hy_s32_t _client_read_ipc_link_msg_thread_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ipc_process_client_context_s *context = args;
    fd_set read_fs = {0};
    struct timeval timeout = {0};
    hy_s32_t ret = 0;
    ipc_link_info_s ipc_link_info;

    ipc_link_info_get(context->ipc_link_h, &ipc_link_info);

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);
        FD_SET(ipc_link_info.fd, &read_fs);

        timeout.tv_sec = 1;
        ret = select(FD_SETSIZE, &read_fs, NULL, NULL, &timeout);
        if (ret < 0) {
            LOGES("select failed \n");
            break;
        }

        if (FD_ISSET(ipc_link_info.fd, &read_fs)) {
            if (0 != _handle_ipc_link_msg(context)) {
                LOGE("handle ipc link msg failed \n");

                // 通知上层断开

                break;
            }
        }
    }

    return -1;
}

static hy_s32_t _ipc_process_client_connect_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ipc_process_client_context_s *context = args;
    ipc_link_info_s ipc_link_info;

    if (0 != ipc_link_connect(context->ipc_link_h, context->connect_timeout_s)) {
        LOGE("ipc link connect failed \n");
        context->is_connect = HY_IPC_PROCESS_CONNECT_STATE_DISCONNECT;
    } else {
        context->is_connect = HY_IPC_PROCESS_CONNECT_STATE_CONNECT;
    }

    context->exit_wait_flag = 1;

    ipc_link_info_get(context->ipc_link_h, &ipc_link_info);

    ipc_link_info_send(context->ipc_link_h, ipc_link_info.tag, context->pid);

    return -1;
}

void ipc_process_client_destroy(void **ipc_process_client_h)
{
    LOGT("&ipc_process_client_h: %p, ipc_process_client_h: %p \n",
            ipc_process_client_h, *ipc_process_client_h);
    HY_ASSERT_RET(!ipc_process_client_h || !*ipc_process_client_h);

    _ipc_process_client_context_s *context = *ipc_process_client_h;

    while (!context->exit_wait_flag) {
        sleep(1);
    }
    HyThreadDestroy(&context->connect_thread_h);
    HyThreadDestroy(&context->read_ipc_link_msg_thread_h);

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

        HY_MEMCPY(&context->save_c,
                &ipc_process_c->save_config, sizeof(context->save_c));

        context->connect_timeout_s  = ipc_process_c->connect_timeout_s;
        context->pid                = getpid();

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

        context->connect_thread_h = HyThreadCreate_m("hy_i_p_c_connect",
                _ipc_process_client_connect_cb, context);
        if (!context->connect_thread_h) {
            LOGE("hy thread create m failed \n");
            break;
        }

        context->read_ipc_link_msg_thread_h = HyThreadCreate_m("hy_cl_r_ipc_msg",
                _client_read_ipc_link_msg_thread_cb, context);
        if (!context->read_ipc_link_msg_thread_h) {
            LOGE("hy thread create m failed \n");
            break;
        }

        LOGI("ipc process client create, context: %p \n", context);
        return  context;
    } while (0);

    LOGE("ipc process client create failed \n");
    ipc_process_client_destroy((void **)&context);
    return NULL;
}

