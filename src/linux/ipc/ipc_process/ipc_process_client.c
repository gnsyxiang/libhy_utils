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

#include "hy_fifo.h"

#include "ipc_process_client.h"
#include "ipc_link.h"

typedef struct {
    HyIpcProcessSaveConfig_s        save_c; // 必须放在前面，用于强制类型转换

    HyIpcProcessFunc_s              *func;
    void                            *func_args;
    hy_u32_t                        func_cnt;

    pid_t                           pid;
    hy_u32_t                        connect_timeout_s;
    void                            *ipc_link_h;

    void                            *ack_fifo_h;
    pthread_cond_t                  ack_cond;
    pthread_mutex_t                 ack_mutex;

    void                            *read_ipc_link_msg_thread_h;
    void                            *connect_thread_h;
    hy_s32_t                        exit_flag:1;
    hy_s32_t                        exit_wait_flag:1;
    HyIpcProcessConnectState_e      is_connect:2;
    hy_s32_t                        reserved;
} _ipc_process_client_context_s;

static hy_s32_t _wait_ack(_ipc_process_client_context_s *context,
        void *recv, hy_u32_t recv_len)
{
    ipc_link_msg_s ipc_msg;
    HyIpcProcessMsgId_e id;
    hy_s32_t result = 0;
    hy_u32_t offset = 0;
    hy_u32_t len;

    offset = sizeof(id) + sizeof(result) + sizeof(recv_len);

    pthread_mutex_lock(&context->ack_mutex);

    len = HyFifoGetInfo(context->ack_fifo_h, HY_FIFO_INFO_USED_LEN);
    while (len < sizeof(ipc_msg)) {
        pthread_cond_wait(&context->ack_cond, &context->ack_mutex);
        HyFifoReadPeek(context->ack_fifo_h, &ipc_msg, sizeof(ipc_msg));

        if (ipc_msg.buf_len == recv_len + offset) {
            break;
        }
    }

    HyFifoRead(context->ack_fifo_h, &ipc_msg, sizeof(ipc_msg));
    HyFifoUpdateOut(context->ack_fifo_h, offset);
    HyFifoRead(context->ack_fifo_h, recv, recv_len);

    pthread_mutex_unlock(&context->ack_mutex);

    return 0;
}

hy_s32_t ipc_process_client_data_sync(void *ipc_process_client_h,
        HyIpcProcessMsgId_e id, void *send, hy_u32_t send_len,
        void *recv, hy_u32_t recv_len)
{
    LOGT("ipc_process_client_h: %p \n", ipc_process_client_h);
    HY_ASSERT_RET_VAL(!ipc_process_client_h, -1);

    ipc_link_msg_s *ipc_link_msg = NULL;
    _ipc_process_client_context_s *context = ipc_process_client_h;
    hy_u32_t total_len = 0;
    hy_u32_t len = 0;
    hy_s32_t offset = 0;

    total_len = sizeof(ipc_link_msg_s) + sizeof(id)
        + sizeof(send_len) + sizeof(recv_len) + send_len + recv_len;

    ipc_link_msg = HY_MEM_MALLOC_RET_VAL(ipc_link_msg_s *, total_len, -1);

    len = sizeof(id);
    HY_MEMCPY(ipc_link_msg->buf + offset, &id, len);
    offset += len;

    len = sizeof(send_len);
    HY_MEMCPY(ipc_link_msg->buf + offset, &send_len, len);
    offset += len;

    len = sizeof(recv_len);
    HY_MEMCPY(ipc_link_msg->buf + offset, &recv_len, len);
    offset += len;

    len = send_len;
    HY_MEMCPY(ipc_link_msg->buf + offset, send, len);
    offset += len;

    len = recv_len;
    HY_MEMCPY(ipc_link_msg->buf + offset, recv, len);
    offset += len;

    ipc_link_msg->total_len     = total_len;
    ipc_link_msg->type          = IPC_LINK_MSG_TYPE_CB;
    ipc_link_msg->buf_len       = offset;

    ipc_link_write(context->ipc_link_h, ipc_link_msg, 1);

    return _wait_ack(context, recv, recv_len);
}

static hy_s32_t ipc_process_client_send_ack(
        _ipc_process_client_context_s *context, void *ipc_link_h,
        HyIpcProcessMsgId_e id,
        hy_s32_t result, const void *buf, hy_u32_t buf_len)
{
    ipc_link_msg_s *ipc_link_msg = NULL;
    hy_u32_t total_len = 0;
    hy_u32_t len = 0;
    hy_s32_t offset = 0;

    total_len = sizeof(ipc_link_msg_s) + sizeof(id)
        + sizeof(result) + sizeof(buf_len) + buf_len;

    ipc_link_msg = HY_MEM_MALLOC_RET_VAL(ipc_link_msg_s *, total_len, -1);

    len = sizeof(id);
    HY_MEMCPY(ipc_link_msg->buf + offset, &id, len);
    offset += len;

    len = sizeof(result);
    HY_MEMCPY(ipc_link_msg->buf + offset, &result, len);
    offset += len;

    len = sizeof(buf_len);
    HY_MEMCPY(ipc_link_msg->buf + offset, &buf_len, len);
    offset += len;

    len = buf_len;
    HY_MEMCPY(ipc_link_msg->buf + offset, buf, buf_len);
    offset += len;

    ipc_link_msg->total_len = total_len;
    ipc_link_msg->type      = IPC_LINK_MSG_TYPE_ACK;
    ipc_link_msg->buf_len   = offset;
    ipc_link_msg->ipc_link_h= ipc_link_h;

    return ipc_link_write(context->ipc_link_h, ipc_link_msg, 1);
}

static void _handle_ipc_link_msg_info(_ipc_process_client_context_s *context,
        ipc_link_msg_s *ipc_msg)
{
    const char *tag = NULL;
    pid_t pid;
    ipc_link_info_s ipc_link_info;
    HyIpcProcessInfo_s ipc_process_info;
    HyIpcProcessSaveConfig_s *save_c = &context->save_c;

    pid = *(pid_t *)ipc_msg->buf;
    tag = (const char *)(ipc_msg->buf + sizeof(pid_t));

    ipc_link_info_get(context->ipc_link_h, &ipc_link_info);

    ipc_process_info.pid        = pid;
    ipc_process_info.tag        = tag;
    ipc_process_info.ipc_name   = ipc_link_info.ipc_name;

    if (save_c->state_change_cb) {
        save_c->state_change_cb(&ipc_process_info,
                HY_IPC_PROCESS_CONNECT_STATE_CONNECT, save_c->args);
    }
}

static void _handle_ipc_link_msg_cb(_ipc_process_client_context_s *context,
        ipc_link_msg_s *ipc_msg)
{
    HyIpcProcessMsgId_e id;
    hy_u32_t send_len;
    hy_u32_t recv_len = -1;
    void *send = NULL;
    void *recv = NULL;
    hy_s32_t ret = -1;

    id = *(HyIpcProcessMsgId_e *)ipc_msg->buf;
    send_len = *(hy_u32_t *)(ipc_msg->buf + sizeof(id));
    recv_len = *(hy_u32_t *)(ipc_msg->buf + sizeof(id) + sizeof(send_len));

    send = (void *)(ipc_msg->buf + sizeof(id) + sizeof(send_len) + sizeof(recv_len));
    recv = (void *)(ipc_msg->buf + sizeof(id) + sizeof(send_len) + sizeof(recv_len) + send_len);

    LOGE("id: %d, send_len: %d, recv_len: %d \n", id, send_len, recv_len);

    for (hy_u32_t i = 0; i < context->func_cnt; ++i) {
        if (id == context->func[i].id && context->func[i].func_cb) {
            ret = context->func[i].func_cb(send,
                    send_len, recv, recv_len, context->func_args);
            break;
        }
    }

    ipc_process_client_send_ack(context, ipc_msg->ipc_link_h,
            id, ret, recv, recv_len);
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
            if (ipc_msg) {
                HY_MEM_FREE_P(ipc_msg);
            }
            break;
        case IPC_LINK_MSG_TYPE_ACK:
            LOGE("--------1------------ack \n");
            pthread_mutex_lock(&context->ack_mutex);
            HyFifoWrite(context->ack_fifo_h, ipc_msg, ipc_msg->total_len);
            pthread_mutex_unlock(&context->ack_mutex);

            LOGE("--------2------------ack \n");
            pthread_cond_signal(&context->ack_cond);
            LOGE("--------3------------ack \n");
            if (ipc_msg) {
                HY_MEM_FREE_P(ipc_msg);
            }
            break;
        case IPC_LINK_MSG_TYPE_CB:
            LOGE("-----------------cb\n");
            _handle_ipc_link_msg_cb(context, ipc_msg);
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
    HyIpcProcessSaveConfig_s *save_c = &context->save_c;
    ipc_link_info_s ipc_link_info;
    fd_set read_fs = {0};
    struct timeval timeout = {0};
    hy_s32_t ret = 0;

    while (HY_IPC_PROCESS_CONNECT_STATE_CONNECT != context->is_connect) {
        usleep(10 * 1000);
    }

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

                ipc_link_info_get(context->ipc_link_h, &ipc_link_info);

                if (save_c->state_change_cb) {
                    save_c->state_change_cb(NULL,
                            HY_IPC_PROCESS_CONNECT_STATE_DISCONNECT,
                            save_c->args);
                }

                break;
            }
        }
    }

    return -1;
}

static hy_s32_t _ipc_process_client_send_cb_id(_ipc_process_client_context_s
        *context)
{
    ipc_link_msg_s *ipc_link_msg = NULL;
    hy_u32_t total_len = 0;
    hy_u32_t len = 0;
    hy_s32_t offset = 0;
    hy_u32_t *id = NULL;

    total_len = sizeof(ipc_link_msg_s) + sizeof(hy_u32_t)
        + sizeof(hy_s32_t) * context->func_cnt;
    ipc_link_msg = HY_MEM_MALLOC_RET_VAL(ipc_link_msg_s *, total_len, -1);

    len = sizeof(hy_u32_t);
    HY_MEMCPY(ipc_link_msg->buf + offset, &context->func_cnt, len);
    offset += len;

    len = sizeof(hy_s32_t) * context->func_cnt;
    id = (hy_u32_t *)(ipc_link_msg->buf + offset);
    for (hy_u32_t i = 0; i < context->func_cnt; ++i) {
        id[i] = context->func[i].id;
    }
    offset += len;

    ipc_link_msg->total_len  = total_len;
    ipc_link_msg->type       = IPC_LINK_MSG_TYPE_CB_ID;
    ipc_link_msg->buf_len    = offset;

    return ipc_link_write(context->ipc_link_h, ipc_link_msg, 1);
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

    _ipc_process_client_send_cb_id(context);

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

    context->exit_flag = 1;
    HyThreadDestroy(&context->read_ipc_link_msg_thread_h);

    if (context->func_cnt) {
        HY_MEM_FREE_PP(&context->func);
    }

    pthread_cond_destroy(&context->ack_cond);
    pthread_mutex_destroy(&context->ack_mutex);
    HyFifoDestroy(&context->ack_fifo_h);

    ipc_link_destroy(&context->ipc_link_h);

    LOGI("ipc process client destroy, context: %p \n", context);
    HY_MEM_FREE_PP(ipc_process_client_h);
}

void *ipc_process_client_create(HyIpcProcessConfig_s *ipc_process_c)
{
    LOGT("ipc_process_c: %p \n", ipc_process_c);
    HY_ASSERT_RET_VAL(!ipc_process_c, NULL);

    _ipc_process_client_context_s *context = NULL;
    hy_u32_t len = 0;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_process_client_context_s *,
                sizeof(*context));

        HY_MEMCPY(&context->save_c,
                &ipc_process_c->save_config, sizeof(context->save_c));

        context->connect_timeout_s  = ipc_process_c->connect_timeout_s;
        context->pid                = getpid();

        pthread_mutex_init(&context->ack_mutex, NULL);
        pthread_cond_init(&context->ack_cond, NULL);
        context->ack_fifo_h = HyFifoCreate_m(2 * 1024, HY_FIFO_MUTEX_UNLOCK);
        if (!context->ack_fifo_h) {
            LOGE("hy fifo create failed \n");
            break;
        }

        if (ipc_process_c->func_cnt) {
            len = sizeof(HyIpcProcessFunc_s) * ipc_process_c->func_cnt;
            context->func = HY_MEM_MALLOC_BREAK(HyIpcProcessFunc_s *, len);

            HY_MEMCPY(context->func, ipc_process_c->func, len);
            context->func_cnt = ipc_process_c->func_cnt;
            context->func_args = ipc_process_c->func_args;
        }

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

