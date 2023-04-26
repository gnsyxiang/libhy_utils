/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_server.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 16:20
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 16:20
 */
#include <stdio.h>
#include <semaphore.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_thread.h"
#include "hy_pipe.h"
#include "hy_fifo.h"

#include "ipc_process_server.h"
#include "ipc_link_manager.h"
#include "ipc_link.h"

typedef struct {
    struct hy_list_head             entry;

    hy_u32_t                        cnt;
    hy_u32_t                        *id;
} _func_cb_id_s;

typedef struct {
    void                            *ipc_link_h;
    ipc_link_msg_s                  *ipc_msg;
} _func_cb_s;

typedef struct {
    HyIpcProcessSaveConfig_s        save_config;// 必须放在前面，用于强制类型转换

    pid_t                           pid;
    void                            *ipc_link_h;
    void                            *ipc_link_manager_h;

    pthread_mutex_t                 cb_id_mutex;
    struct hy_list_head             cb_id_list;

    sem_t                           cb_sem;
    void                            *cb_fifo_h;
    void                            *handle_cb_thread_h;

    sem_t                           ack_sem;
    void                            *ack_fifo_h;
    void                            *handle_ack_thread_h;

    void                            *pipe_h;
    void                            *handle_ipc_link_msg_thread_h;

    hy_s32_t                        exit_flag:1;
    hy_s32_t                        reserved;
} _ip_server_context_s;

static void _server_handle_ipc_link_msg_info(_ip_server_context_s *context,
        ipc_link_msg_s *ipc_msg)
{
    LOGT("context: %p, ipc_msg: %p \n", context, ipc_msg);
    HY_ASSERT_RET(!context || !ipc_msg);

    ipc_link_info_s ipc_link_info;
    HyIpcProcessInfo_s ipc_process_info;
    HyIpcProcessSaveConfig_s *save_c = &context->save_config;

    ipc_link_get_info(context->ipc_link_h, &ipc_link_info);

    ipc_process_info.pid        = *(pid_t *)ipc_msg->buf;
    ipc_process_info.tag        = (const char *)(ipc_msg->buf + sizeof(pid_t));
    ipc_process_info.ipc_name   = ipc_link_info.ipc_name;

    if (save_c->state_change_cb) {
        save_c->state_change_cb(&ipc_process_info,
                HY_IPC_PROCESS_CONNECT_STATE_CONNECT, save_c->args);
    }
}

static void _server_handle_ipc_link_msg_cb_info(_ip_server_context_s *context,
        ipc_link_msg_s *ipc_msg)
{
    LOGT("context: %p, ipc_msg: %p \n", context, ipc_msg);
    HY_ASSERT_RET(!context || !ipc_msg);

    hy_u32_t cnt = 0;
    hy_u32_t *id = NULL;
    _func_cb_id_s *func_cb_id = NULL;

    cnt = *(hy_u32_t *)ipc_msg->buf;

    func_cb_id = HY_MEM_MALLOC_RET(_func_cb_id_s *, sizeof(*func_cb_id));
    func_cb_id->id = HY_MEM_MALLOC_RET(hy_u32_t *, cnt * sizeof(hy_u32_t));

    func_cb_id->cnt = cnt;

    id = (hy_u32_t *)(ipc_msg->buf + sizeof(cnt));
    for (hy_u32_t i = 0; i < cnt; ++i) {
        func_cb_id->id[i] = id[i];
    }

    pthread_mutex_lock(&context->cb_id_mutex);
    hy_list_add_tail(&func_cb_id->entry, &context->cb_id_list);
    pthread_mutex_unlock(&context->cb_id_mutex);
}

static hy_s32_t _server_handle_ipc_link_msg(
        ipc_link_manager_list_s *ipc_link_list, _ip_server_context_s *context)
{
    LOGT("ipc_link_list: %p, context: %p \n", ipc_link_list, context);
    HY_ASSERT_RET_VAL(!ipc_link_list || !context, -1);

    ipc_link_msg_s *ipc_msg = NULL;
    _func_cb_s func_cb;
    hy_s32_t flag = 0;

    if (0 != ipc_link_read(ipc_link_list->ipc_link_h, &ipc_msg)) {
        LOGE("ipc link read failed \n");
        return -1;
    }

    switch (ipc_msg->type) {
        case IPC_LINK_MSG_TYPE_INFO:
            _server_handle_ipc_link_msg_info(context, ipc_msg);
            flag = 1;
            break;
        case IPC_LINK_MSG_TYPE_ACK:
            func_cb.ipc_link_h  = ipc_link_list->ipc_link_h;
            func_cb.ipc_msg     = ipc_msg;
            if (0 == HyFifoWrite(context->ack_fifo_h, &func_cb, sizeof(func_cb))) {
                LOGE("hy fifo write failed, lost data \n");
                break;
            }

            sem_post(&context->ack_sem);
            break;
        case IPC_LINK_MSG_TYPE_CB:
            func_cb.ipc_link_h  = ipc_link_list->ipc_link_h;
            func_cb.ipc_msg     = ipc_msg;
            if (0 == HyFifoWrite(context->cb_fifo_h, &func_cb, sizeof(func_cb))) {
                LOGE("hy fifo write failed, lost data \n");
                break;
            }

            sem_post(&context->cb_sem);
            break;
        case IPC_LINK_MSG_TYPE_CB_ID:
            _server_handle_ipc_link_msg_cb_info(context, ipc_msg);
            flag = 1;
            break;
        default:
            break;
    }

    if (flag) {
        if (ipc_msg) {
            HY_MEM_FREE_P(ipc_msg);
        }
    }

    return 0;
}

static hy_s32_t _handle_cb_thread_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ip_server_context_s *context = args;
    struct hy_list_head *ipc_link_list = NULL;
    ipc_link_manager_list_s *pos;
    _func_cb_s func_cb;
    _func_cb_id_s *offset;

    while (!context->exit_flag) {
        sem_wait(&context->cb_sem);

        if (context->exit_flag) {
            break;
        }

        HyFifoRead(context->cb_fifo_h, &func_cb, sizeof(func_cb));

        ipc_link_list = ipc_link_manager_list_get(context->ipc_link_manager_h);
        hy_list_for_each_entry(pos, ipc_link_list, entry) {
            if (pos->ipc_link_h == func_cb.ipc_link_h) {
                LOGT("save ipc link \n");
                continue;
            }

            {
                pthread_mutex_lock(&context->cb_id_mutex);
                hy_list_for_each_entry(offset, &context->cb_id_list, entry) {
                    for (hy_u32_t i = 0; i < offset->cnt; ++i) {
                        if (func_cb.ipc_msg->id != offset->id[i]) {
                            continue;
                        }
                        LOGT("contain msg id, ipc_link_h: %p, msg id: %d \n",
                                pos->ipc_link_h, func_cb.ipc_msg->id);

                        func_cb.ipc_msg->ipc_link_h = func_cb.ipc_link_h;
                        ipc_link_write(pos->ipc_link_h, func_cb.ipc_msg, 0);
                        break;
                    }
                }
                pthread_mutex_unlock(&context->cb_id_mutex);
            }
        }
        ipc_link_manager_list_put(context->ipc_link_manager_h);

        if (func_cb.ipc_msg) {
            HY_MEM_FREE_P(func_cb.ipc_msg);
        }
    }

    return -1;
}

static hy_s32_t _handle_ack_thread_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ip_server_context_s *context = args;
    struct hy_list_head *ipc_link_list = NULL;
    ipc_link_manager_list_s *pos;
    _func_cb_s func_cb;

    while (!context->exit_flag) {
        sem_wait(&context->ack_sem);

        if (context->exit_flag) {
            break;
        }

        HyFifoRead(context->ack_fifo_h, &func_cb, sizeof(func_cb));

        ipc_link_list = ipc_link_manager_list_get(context->ipc_link_manager_h);
        hy_list_for_each_entry(pos, ipc_link_list, entry) {
            if (pos->ipc_link_h != func_cb.ipc_msg->ipc_link_h) {
                LOGT("can't find ipc link \n");
                continue;
            }

            ipc_link_write(pos->ipc_link_h, func_cb.ipc_msg, 1);
        }
        ipc_link_manager_list_put(context->ipc_link_manager_h);
    }

    return -1;
}

static void _ipc_link_manager_accept_cb(void *ipc_link_h, void *args)
{
    LOGT("ipc_link_h: %p, args: %p \n", ipc_link_h, args);
    HY_ASSERT_RET(!ipc_link_h || !args);

    LOGI("ipc process server accept new ipc_link: %p \n", ipc_link_h);

    _ip_server_context_s *context = args;

    HyPipeWrite(context->pipe_h, &ipc_link_h, sizeof(ipc_link_h));
}

static hy_s32_t _server_handle_ipc_link_msg_thread_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    fd_set read_fs;
    struct timeval timeout = {0};
    void *ipc_link_h = NULL;
    _ip_server_context_s *context = args;
    ipc_link_info_s ipc_link_info;
    struct hy_list_head *ipc_link_list = NULL;
    ipc_link_manager_list_s *pos, *n;
    hy_s32_t fd = -1;
    hy_s32_t pfd = -1;

    memset(&read_fs, '\0', sizeof(read_fs));

    pfd = HyPipeReadFdGet(context->pipe_h);

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);

        FD_SET(pfd, &read_fs);

        ipc_link_list = ipc_link_manager_list_get(context->ipc_link_manager_h);
        hy_list_for_each_entry(pos, ipc_link_list, entry) {
            fd = ipc_link_get_fd(pos->ipc_link_h);

            FD_SET(fd, &read_fs);
        }
        ipc_link_manager_list_put(context->ipc_link_manager_h);

        timeout.tv_sec = 1;
        if (select(FD_SETSIZE, &read_fs, NULL, NULL, &timeout) < 0) {
            LOGES("select failed \n");
            break;
        }

        if (FD_ISSET(pfd, &read_fs)) {
            LOGI("ipc process server send info to client \n");

            HyPipeRead(context->pipe_h, &ipc_link_h, sizeof(ipc_link_h));
            ipc_link_get_info(context->ipc_link_h, &ipc_link_info);
            ipc_link_send_info(ipc_link_h, ipc_link_info.tag, context->pid);
        }

        ipc_link_list = ipc_link_manager_list_get(context->ipc_link_manager_h);
        hy_list_for_each_entry_safe(pos, n, ipc_link_list, entry) {
            fd = ipc_link_get_fd(pos->ipc_link_h);

            if (FD_ISSET(fd, &read_fs)) {
                if (-1 == _server_handle_ipc_link_msg(pos, context)) {
                    hy_list_del(&pos->entry);

                    ipc_link_destroy(&pos->ipc_link_h);
                    HY_MEM_FREE_PP(&pos);
                }
            }
        }
        ipc_link_manager_list_put(context->ipc_link_manager_h);
    }

    return -1;
}

void ipc_process_server_destroy(void **ipc_process_server_h)
{
    LOGT("&ipc_process_server_h: %p, ipc_process_server_h: %p \n",
            ipc_process_server_h, *ipc_process_server_h);
    HY_ASSERT_RET(!ipc_process_server_h || !*ipc_process_server_h);

    _ip_server_context_s *context = *ipc_process_server_h;
    _func_cb_id_s *pos, *n;

    ipc_link_destroy(&context->ipc_link_h);

    context->exit_flag = 1;

    sem_post(&context->ack_sem);
    HyThreadDestroy(&context->handle_ack_thread_h);
    sem_destroy(&context->ack_sem);
    HyFifoDestroy(&context->ack_fifo_h);

    HyThreadDestroy(&context->handle_ipc_link_msg_thread_h);

    ipc_link_manager_destroy(&context->ipc_link_manager_h);

    sem_post(&context->cb_sem);
    HyThreadDestroy(&context->handle_cb_thread_h);
    sem_destroy(&context->cb_sem);
    pthread_mutex_lock(&context->cb_id_mutex);
    hy_list_for_each_entry_safe(pos, n, &context->cb_id_list, entry) {
        hy_list_del(&pos->entry);

        HY_MEM_FREE_P(pos->id);
        HY_MEM_FREE_PP(&pos);
    }
    pthread_mutex_unlock(&context->cb_id_mutex);
    HyFifoDestroy(&context->cb_fifo_h);

    HyPipeDestroy(&context->pipe_h);

    LOGI("ipc process server destroy, context: %p \n", context);
    HY_MEM_FREE_PP(ipc_process_server_h);
}

void *ipc_process_server_create(HyIpcProcessConfig_s *ipc_process_c)
{
    LOGT("ipc_process_c: %p \n", ipc_process_c);
    HY_ASSERT_RET_VAL(!ipc_process_c, NULL);

    _ip_server_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ip_server_context_s *, sizeof(*context));

        HY_MEMCPY(&context->save_config,
                &ipc_process_c->save_config, sizeof(context->save_config));

        context->pid = getpid();

        HY_INIT_LIST_HEAD(&context->cb_id_list);
        pthread_mutex_init(&context->cb_id_mutex, NULL);

        context->pipe_h = HyPipeCreate_m(HY_PIPE_BLOCK_STATE_NOBLOCK);
        if (!context->pipe_h) {
            LOGE("hy pipe create m failed \n");
            break;
        }

        sem_init(&context->cb_sem, 0, 0);
        context->cb_fifo_h = HyFifoCreate_m(sizeof(_func_cb_s) * 1024,
                HY_FIFO_MUTEX_LOCK);
        if (!context->cb_fifo_h) {
            LOGE("hy fifo create failed \n");
            break;
        }

        sem_init(&context->ack_sem, 0, 0);
        context->ack_fifo_h = HyFifoCreate_m(sizeof(_func_cb_s) * 1024,
                HY_FIFO_MUTEX_LOCK);
        if (!context->ack_fifo_h) {
            LOGE("hy fifo create failed \n");
            break;
        }

        context->ipc_link_h = ipc_link_create_m(ipc_process_c->ipc_name,
                ipc_process_c->tag, IPC_LINK_TYPE_SERVER, NULL);
        if (!context->ipc_link_h) {
            LOGE("ipc link create m failed \n");
            break;
        }

        context->ipc_link_manager_h = ipc_link_manager_create_m(
                _ipc_link_manager_accept_cb, context, context->ipc_link_h);
        if (!context->ipc_link_manager_h) {
            LOGE("ipc link manager create faeild \n");
            break;
        }

        context->handle_ipc_link_msg_thread_h = HyThreadCreate_m("HYIPS_handle_msg",
                _server_handle_ipc_link_msg_thread_cb, context);
        if (!context->handle_ipc_link_msg_thread_h) {
            LOGE("hy thread create m failed \n");
            break;
        }

        context->handle_cb_thread_h = HyThreadCreate_m("HYIPS_handle_cb",
                _handle_cb_thread_cb, context);
        if (!context->handle_cb_thread_h) {
            LOGE("hy thread create m failed \n");
            break;
        }

        context->handle_ack_thread_h = HyThreadCreate_m("HYIPS_handle_ack",
                _handle_ack_thread_cb, context);
        if (!context->handle_ack_thread_h) {
            LOGE("hy thread create m failed \n");
            break;
        }

        LOGI("ipc process server create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc process server create failed \n");
    ipc_process_server_destroy((void **)&context);
    return NULL;
}

