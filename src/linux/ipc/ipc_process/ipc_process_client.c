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
#include <sys/time.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_thread.h"

#include "hy_fifo.h"
#include "hy_list.h"

#include "ipc_process_client.h"
#include "ipc_link.h"

typedef struct {
    struct hy_list_head             entry;

    struct timeval                  tv;
    ipc_link_msg_s                  *ipc_msg;
} _ack_node_s;

typedef struct {
    HyIpcProcessSaveConfig_s        save_c; // 必须放在前面，用于强制类型转换

    HyIpcProcessFunc_s              *func;
    void                            *func_args;
    hy_u32_t                        func_cnt;

    pid_t                           pid;
    hy_u32_t                        connect_timeout_s;
    void                            *connect_thread_h;

    void                            *ipc_link_h;

    struct hy_list_head             ack_list;
    pthread_cond_t                  ack_cond;
    pthread_mutex_t                 ack_mutex;

    void                            *handle_ipc_link_msg_thread_h;

    hy_s32_t                        exit_flag:1;
    hy_s32_t                        exit_wait_flag:1;
    HyIpcProcessConnectState_e      is_connect:2;
    hy_s32_t                        reserved;
} _ip_client_context_s;

static hy_s32_t _client_wait_ack(_ip_client_context_s *context,
        HyIpcProcessMsgId_e id, void *recv, hy_u32_t recv_len)
{
    LOGT("context: %p, id: %d, recv: %p recv_len: %d \n",
            context, id, recv, recv_len);
    HY_ASSERT_RET_VAL(!context || !recv, -1);

    struct timespec timeout;
    struct timeval tv;
    hy_u32_t time_s;
    hy_s32_t ret = 0;
    _ack_node_s *pos, *n;

    gettimeofday(&tv, NULL);
    timeout.tv_sec = tv.tv_sec + 3;
    timeout.tv_nsec = tv.tv_usec * 1000;

    pthread_mutex_lock(&context->ack_mutex);
    while (!context->exit_flag) {
        if (hy_list_empty(&context->ack_list)) {
            ret = pthread_cond_timedwait(&context->ack_cond,
                    &context->ack_mutex, &timeout);
            if (ret == ETIMEDOUT) {
                LOGW("wait ack timeout, id: %d \n", id);

                pthread_mutex_unlock(&context->ack_mutex);
                return id;
            }
        } else {
            hy_list_for_each_entry_safe(pos, n, &context->ack_list, entry) {
                if (pos->ipc_msg->id == id) {
                    hy_list_del(&pos->entry);

                    HY_MEMCPY(recv, pos->ipc_msg->buf + sizeof(hy_s32_t) * 2, recv_len);

                    HY_MEM_FREE_PP(&pos->ipc_msg);
                    HY_MEM_FREE_PP(&pos);
                    pthread_mutex_unlock(&context->ack_mutex);
                    return 0;
                } else {
                    time_s = (tv.tv_sec - pos->tv.tv_sec);
                    if (time_s > 3) {
                        LOGD("ack timeout, id: %d \n", id);

                        hy_list_del(&pos->entry);

                        HY_MEM_FREE_PP(&pos->ipc_msg);
                        HY_MEM_FREE_PP(&pos);
                    }
                }
            }
        }
    }

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
    _ip_client_context_s *context = ipc_process_client_h;
    hy_u32_t total_len = 0;
    hy_u32_t len = 0;
    hy_s32_t offset = 0;

    total_len += sizeof(ipc_link_msg_s);
    total_len += sizeof(send_len) + send_len;
    total_len += sizeof(recv_len) + recv_len;

    ipc_link_msg = HY_MEM_MALLOC_RET_VAL(ipc_link_msg_s *, total_len, -1);

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
    ipc_link_msg->id            = id;

    len = ipc_link_write(context->ipc_link_h, ipc_link_msg, 1);
    if (0 != len) {
        LOGE("ipc link write failed \n");
        return -1;
    } else {
        return _client_wait_ack(context, id, recv, recv_len);
    }
}

static hy_s32_t _client_send_ack(_ip_client_context_s *context,
        void *ipc_link_h, HyIpcProcessMsgId_e id,
        hy_s32_t result, const void *buf, hy_u32_t buf_len)
{
    LOGT("context: %p, ipc_link_h: %p, id: %d, result: %d, buf: %p, buf_len: %d \n",
            context, ipc_link_h, id, result, buf, buf_len);
    HY_ASSERT_RET_VAL(!context || !ipc_link_h || !buf, -1);

    ipc_link_msg_s *ipc_link_msg = NULL;
    hy_u32_t total_len = 0;
    hy_u32_t len = 0;
    hy_s32_t offset = 0;

    total_len += sizeof(ipc_link_msg_s);
    total_len += sizeof(result);
    total_len += sizeof(buf_len) + buf_len;

    ipc_link_msg = HY_MEM_MALLOC_RET_VAL(ipc_link_msg_s *, total_len, -1);

    len = sizeof(result);
    HY_MEMCPY(ipc_link_msg->buf + offset, &result, len);
    offset += len;

    len = sizeof(buf_len);
    HY_MEMCPY(ipc_link_msg->buf + offset, &buf_len, len);
    offset += len;

    len = buf_len;
    HY_MEMCPY(ipc_link_msg->buf + offset, buf, buf_len);
    offset += len;

    ipc_link_msg->total_len     = total_len;
    ipc_link_msg->type          = IPC_LINK_MSG_TYPE_ACK;
    ipc_link_msg->buf_len       = offset;
    ipc_link_msg->ipc_link_h    = ipc_link_h;
    ipc_link_msg->id            = id;

    return ipc_link_write(context->ipc_link_h, ipc_link_msg, 1);
}

static void _client_handle_ipc_link_msg_info(_ip_client_context_s *context,
        ipc_link_msg_s *ipc_msg)
{
    LOGT("context: %p, ipc_msg: %p \n", context, ipc_msg);
    HY_ASSERT_RET(!context || !ipc_msg);

    ipc_link_info_s ipc_link_info;
    HyIpcProcessInfo_s ipc_process_info;
    HyIpcProcessSaveConfig_s *save_c = &context->save_c;

    ipc_link_get_info(context->ipc_link_h, &ipc_link_info);

    ipc_process_info.pid        = *(pid_t *)ipc_msg->buf;
    ipc_process_info.tag        = (const char *)(ipc_msg->buf + sizeof(pid_t));
    ipc_process_info.ipc_name   = ipc_link_info.ipc_name;

    if (save_c->state_change_cb) {
        save_c->state_change_cb(&ipc_process_info,
                HY_IPC_PROCESS_CONNECT_STATE_CONNECT, save_c->args);
    }
}

static void _client_handle_ipc_link_msg_cb(_ip_client_context_s *context,
        ipc_link_msg_s *ipc_msg)
{
    LOGT("context: %p, ipc_msg: %p \n", context, ipc_msg);
    HY_ASSERT_RET(!context || !ipc_msg);

    hy_u32_t send_len;
    hy_u32_t recv_len = -1;
    void *send = NULL;
    void *recv = NULL;
    hy_s32_t ret = -1;
    hy_u32_t offset = 0;

    offset += 0;
    send_len = *(hy_u32_t *)(ipc_msg->buf + offset);

    offset += sizeof(send_len);
    recv_len = *(hy_u32_t *)(ipc_msg->buf + offset);

    offset += sizeof(recv_len);
    send = (void *)(ipc_msg->buf + offset);

    offset += send_len;
    recv = (void *)(ipc_msg->buf + offset);


    for (hy_u32_t i = 0; i < context->func_cnt; ++i) {
        if (ipc_msg->id == context->func[i].id && context->func[i].func_cb) {
            ret = context->func[i].func_cb(send, send_len,
                    recv, recv_len, context->func_args);
            break;
        }
    }

    _client_send_ack(context, ipc_msg->ipc_link_h, ipc_msg->id,
            ret, recv, recv_len);
}

static hy_s32_t _client_handle_ipc_link_msg(_ip_client_context_s *context)
{
    LOGT("context: %p \n", context);
    HY_ASSERT_RET_VAL(!context, -1);

    ipc_link_msg_s *ipc_msg = NULL;
    hy_s32_t flag = 0;
    _ack_node_s *ack_node = NULL;

    if (0 != ipc_link_read(context->ipc_link_h, &ipc_msg)) {
        LOGE("ipc link read failed \n");
        return -1;
    }

    switch (ipc_msg->type) {
        case IPC_LINK_MSG_TYPE_INFO:
            _client_handle_ipc_link_msg_info(context, ipc_msg);
            flag = 1;
            break;
        case IPC_LINK_MSG_TYPE_ACK:
            ack_node = HY_MEM_MALLOC_RET_VAL(_ack_node_s *,
                    sizeof(*ack_node), -1);

            ack_node->ipc_msg = ipc_msg;
            gettimeofday(&ack_node->tv, NULL);

            pthread_mutex_lock(&context->ack_mutex);
            hy_list_add_tail(&ack_node->entry, &context->ack_list);
            pthread_mutex_unlock(&context->ack_mutex);

            pthread_cond_signal(&context->ack_cond);
            break;
        case IPC_LINK_MSG_TYPE_CB:
            _client_handle_ipc_link_msg_cb(context, ipc_msg);
            flag = 1;
            break;
        case IPC_LINK_MSG_TYPE_CB_ID:
            break;
        default:
            LOGE("error type \n");
            break;
    }

    if (flag && ipc_msg) {
        HY_MEM_FREE_P(ipc_msg);
    }

    return 0;
}

static hy_s32_t _client_handle_ipc_link_msg_thread_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ip_client_context_s *context = args;
    struct timeval timeout = {0};
    fd_set read_fs = {0};
    hy_s32_t fd = -1;

    while (HY_IPC_PROCESS_CONNECT_STATE_CONNECT != context->is_connect) {
        usleep(10 * 1000);
    }

    fd = ipc_link_get_fd(context->ipc_link_h);

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);
        FD_SET(fd, &read_fs);

        timeout.tv_sec = 1;
        if (select(FD_SETSIZE, &read_fs, NULL, NULL, &timeout) < 0) {
            LOGES("select failed \n");
            break;
        }

        if (FD_ISSET(fd, &read_fs)) {
            if (0 != _client_handle_ipc_link_msg(context)) {
                LOGE("handle ipc link msg failed \n");

                if (context->save_c.state_change_cb) {
                    context->save_c.state_change_cb(NULL,
                            HY_IPC_PROCESS_CONNECT_STATE_DISCONNECT,
                            context->save_c.args);
                }

                break;
            }
        }
    }

    return -1;
}

static hy_s32_t _client_send_cb_id(_ip_client_context_s *context)
{
    LOGT("context: %p \n", context);
    HY_ASSERT_RET_VAL(!context, -1);

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

static hy_s32_t _client_connect_thread_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ip_client_context_s *context = args;
    ipc_link_info_s ipc_link_info;

    if (0 != ipc_link_connect(context->ipc_link_h, context->connect_timeout_s)) {
        LOGE("ipc link connect failed \n");
        context->is_connect = HY_IPC_PROCESS_CONNECT_STATE_DISCONNECT;
    } else {
        context->is_connect = HY_IPC_PROCESS_CONNECT_STATE_CONNECT;
    }

    context->exit_wait_flag = 1;

    ipc_link_get_info(context->ipc_link_h, &ipc_link_info);
    ipc_link_send_info(context->ipc_link_h, ipc_link_info.tag, context->pid);

    _client_send_cb_id(context);

    return -1;
}

void ipc_process_client_destroy(void **ipc_process_client_h)
{
    LOGT("&ipc_process_client_h: %p, ipc_process_client_h: %p \n",
            ipc_process_client_h, *ipc_process_client_h);
    HY_ASSERT_RET(!ipc_process_client_h || !*ipc_process_client_h);

    _ip_client_context_s *context = *ipc_process_client_h;
    _ack_node_s *pos, *n;

    while (!context->exit_wait_flag) {
        usleep(10 * 1000);
    }
    HyThreadDestroy(&context->connect_thread_h);

    context->exit_flag = 1;
    HyThreadDestroy(&context->handle_ipc_link_msg_thread_h);

    if (context->func_cnt) {
        HY_MEM_FREE_PP(&context->func);
    }

    pthread_cond_signal(&context->ack_cond);
    pthread_cond_destroy(&context->ack_cond);
    pthread_mutex_destroy(&context->ack_mutex);
    hy_list_for_each_entry_safe(pos, n, &context->ack_list, entry) {
        hy_list_del(&pos->entry);

        HY_MEM_FREE_PP(&pos->ipc_msg);
        HY_MEM_FREE_PP(&pos);
    }

    ipc_link_destroy(&context->ipc_link_h);

    LOGI("ipc process client destroy, context: %p \n", context);
    HY_MEM_FREE_PP(ipc_process_client_h);
}

void *ipc_process_client_create(HyIpcProcessConfig_s *ipc_process_c)
{
    LOGT("ipc_process_c: %p \n", ipc_process_c);
    HY_ASSERT_RET_VAL(!ipc_process_c, NULL);

    _ip_client_context_s *context = NULL;
    hy_u32_t len = 0;

    do {
        context = HY_MEM_MALLOC_BREAK(_ip_client_context_s *, sizeof(*context));

        HY_MEMCPY(&context->save_c,
                &ipc_process_c->save_config, sizeof(context->save_c));

        context->connect_timeout_s  = ipc_process_c->connect_timeout_s;
        context->pid                = getpid();

        pthread_mutex_init(&context->ack_mutex, NULL);
        pthread_cond_init(&context->ack_cond, NULL);
        HY_INIT_LIST_HEAD(&context->ack_list);

        if (ipc_process_c->func_cnt) {
            len = sizeof(HyIpcProcessFunc_s) * ipc_process_c->func_cnt;
            context->func = HY_MEM_MALLOC_BREAK(HyIpcProcessFunc_s *, len);

            HY_MEMCPY(context->func, ipc_process_c->func, len);
            context->func_cnt = ipc_process_c->func_cnt;
            context->func_args = ipc_process_c->func_args;
        }

        context->ipc_link_h = ipc_link_create_m(ipc_process_c->ipc_name,
                ipc_process_c->tag, IPC_LINK_TYPE_CLIENT, NULL);
        if (!context->ipc_link_h) {
            LOGE("ipc link create failed \n");
            break;
        }

        context->connect_thread_h = HyThreadCreate_m("HYIPC_connect",
                _client_connect_thread_cb, context);
        if (!context->connect_thread_h) {
            LOGE("hy thread create m failed \n");
            break;
        }

        context->handle_ipc_link_msg_thread_h = HyThreadCreate_m("HYIPC_handle_msg",
                _client_handle_ipc_link_msg_thread_cb, context);
        if (!context->handle_ipc_link_msg_thread_h) {
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

