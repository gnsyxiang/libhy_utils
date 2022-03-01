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
#include <semaphore.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_thread.h"

#include "hy_fifo.h"

#include "ipc_link_manager.h"
#include "ipc_process_server.h"

typedef struct {
    HyIpcProcessSaveConfig_s    save_config;

    ipc_link_manager_s          *ipc_link_manager;
    struct hy_list_head         ipc_msg_usr_list;
    struct hy_list_head         id_list;

    sem_t                       msg_sem;

    hy_s32_t                    pfd[2];
    void                        *fifo_read_h;
    void                        *read_msg_thread_h;
    void                        *write_msg_thread_h;
    hy_s32_t                    exit_flag;
} _ipc_process_server_context_s;

hy_s32_t ipc_process_server_write_sync(void *handle, void *msg, hy_u32_t len)
{
    return 0;
}

static void _process_server_handle_callback(ipc_link_msg_handle_s *ipc_msg_usr,
        _ipc_process_server_context_s *context)
{
    LOGT("ipc_link: %p, context: %p \n", ipc_msg_usr, context);
    HY_ASSERT(ipc_msg_usr);
    HY_ASSERT(context);

    ipc_link_s *pos;
    struct hy_list_head *client_link_list = NULL;

    client_link_list = ipc_link_manager_get_list(context->ipc_link_manager);
    hy_list_for_each_entry(pos, client_link_list, entry) {
        if (pos == ipc_msg_usr->ipc_link) {
            continue;
        }

        ipc_link_write(pos, ipc_msg_usr->ipc_msg);
        ipc_msg_usr->ipc_msg = NULL;
    }
    ipc_link_manager_put_list(context->ipc_link_manager);
}

static hy_s32_t _process_server_parse_msg(ipc_link_s *ipc_link,
        _ipc_process_server_context_s *context)
{
    pid_t pid = 0;
    ipc_link_msg_s *ipc_msg = NULL;
    ipc_link_msg_handle_s ipc_link_msg_handle;
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
        case IPC_LINK_MSG_TYPE_CB:
            ipc_link_msg_handle.ipc_link = ipc_link;
            ipc_link_msg_handle.ipc_msg = ipc_msg;

            if (0 == HyFifoWrite(context->fifo_read_h,
                    &ipc_link_msg_handle, sizeof(ipc_link_msg_handle))) {
                LOGE("hy fifo write failed, lost data \n");
                break;
            }

            sem_post(&context->msg_sem);
            break;
        case IPC_LINK_MSG_TYPE_CB_ID:
            LOGE("-----haha---------cb_id \n");
            break;
        default:
            LOGE("error ipc_msg type\n");
            break;
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

static hy_s32_t _process_server_read_msg_cb(void *args)
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

    LOGI("ipc process server handle msg start \n");

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);
        FD_SET(context->pfd[0], &read_fs);

        client_link_list = ipc_link_manager_get_list(context->ipc_link_manager);
        hy_list_for_each_entry(pos, client_link_list, entry) {
            fd = ipc_link_get_fd(pos);
            FD_SET(fd, &read_fs);
        }
        ipc_link_manager_put_list(context->ipc_link_manager);

        timeout.tv_sec = 1;
        if (select(FD_SETSIZE, &read_fs, NULL, NULL, NULL) < 0) {
            LOGES("select failed \n");
            break;
        }

        if (FD_ISSET(context->pfd[0], &read_fs)) {
            LOGI("ipc process server send info to client \n");

            read(context->pfd[0], &client_link_handle, sizeof(void *));

            ipc_link_write_info(client_link_handle,
                    context->ipc_link_manager->link->tag,
                    context->ipc_link_manager->link->pid);
        }

        client_link_list = ipc_link_manager_get_list(context->ipc_link_manager);
        hy_list_for_each_entry_safe(pos, n, client_link_list, entry) {
            fd = ipc_link_get_fd(pos);

            if (FD_ISSET(fd, &read_fs)) {
                if (-1 == _process_server_parse_msg(pos, context)) {
                    hy_list_del(&pos->entry);
                    ipc_link_destroy(&pos);

                    continue;
                }
            }
        }
        ipc_link_manager_put_list(context->ipc_link_manager);
    }

    LOGI("ipc process server handle msg stop \n");

    return -1;
}

static hy_s32_t _process_server_write_msg_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    ipc_link_msg_handle_s ipc_handle_msg;
    ipc_link_s *pos = NULL;
    struct hy_list_head *client_link_list = NULL;
    _ipc_process_server_context_s *context = args;

    while (!context->exit_flag) {
        sem_wait(&context->msg_sem);

        // 处理ack消息

        // 处理回调消息
        HyFifoRead(context->fifo_read_h, &ipc_handle_msg, sizeof(ipc_handle_msg));

        client_link_list = ipc_link_manager_get_list(context->ipc_link_manager);
        hy_list_for_each_entry(pos, client_link_list, entry) {
            if (pos == ipc_handle_msg.ipc_link) {
                LOGD("save ipc link \n");
                continue;
            }

            ipc_link_write(pos, ipc_handle_msg.ipc_msg);
            ipc_handle_msg.ipc_msg = NULL;
        }
        ipc_link_manager_put_list(context->ipc_link_manager);

        if (ipc_handle_msg.ipc_msg) {
            free(ipc_handle_msg.ipc_msg);
        }
    }

    return -1;
}

void ipc_process_server_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_process_server_context_s *context = *handle;

    context->exit_flag = 1;
    HyThreadDestroy(&context->read_msg_thread_h);

    ipc_link_manager_destroy(&context->ipc_link_manager);

    HyFifoDestroy(&context->fifo_read_h);
    sem_destroy(&context->msg_sem);

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

        HyIpcProcessSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        HY_INIT_LIST_HEAD(&context->ipc_msg_usr_list);
        HY_INIT_LIST_HEAD(&context->id_list);
        pipe(context->pfd);

        sem_init(&context->msg_sem, 0, 0);

        hy_u32_t len = sizeof(ipc_link_msg_handle_s *) * 1024;
        context->fifo_read_h = HyFifoCreate_m(len, HY_FIFO_MUTEX_LOCK);
        if (!context->fifo_read_h) {
            LOGE("hy fifo create failed \n");
            break;
        }

        context->ipc_link_manager = ipc_link_manager_create(config->ipc_name,
                config->tag, _process_server_link_manager_accept_cb, context);
        if (!context->ipc_link_manager) {
            LOGE("ipc link manager create failed \n");
            break;
        }

        context->read_msg_thread_h = HyThreadCreate_m("hy_i_p_read_msg",
                _process_server_read_msg_cb, context);
        if (!context->read_msg_thread_h) {
            LOGE("hy thread create failed \n");
            break;
        }

        context->write_msg_thread_h = HyThreadCreate_m("hy_i_p_write_msg",
                _process_server_write_msg_cb, context);
        if (!context->write_msg_thread_h) {
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
