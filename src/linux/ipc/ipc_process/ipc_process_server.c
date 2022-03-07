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

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_pipe.h"

#include "ipc_process_server.h"
#include "ipc_link_manager.h"
#include "ipc_link.h"

typedef struct {
    struct hy_list_head             entry;

    hy_u32_t                        cnt;
    hy_u32_t                        *id;
} _func_cb_id_s;

typedef struct {
    HyIpcProcessSaveConfig_s        save_config;// 必须放在前面，用于强制类型转换

    pid_t                           pid;
    void                            *ipc_link_h;
    void                            *ipc_link_manager_h;

    pthread_mutex_t                 func_id_mutex;
    struct hy_list_head             func_id_list;

    void                            *pipe_h;
    void                            *read_ipc_link_msg_thread_h;
    hy_s32_t                        exit_flag:1;
    hy_s32_t                        reserved;
} _ipc_process_server_context_s;

static void _handle_ipc_link_msg_info(_ipc_process_server_context_s *context,
        ipc_link_msg_s *ipc_msg)
{
    const char *tag = NULL;
    pid_t pid;
    ipc_link_info_s ipc_link_info;
    HyIpcProcessInfo_s ipc_process_info;
    HyIpcProcessSaveConfig_s *save_c = &context->save_config;

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

static void _handle_ipc_link_msg_cb_id(_ipc_process_server_context_s *context,
        ipc_link_msg_s *ipc_msg)
{
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

    pthread_mutex_lock(&context->func_id_mutex);
    hy_list_add_tail(&func_cb_id->entry, &context->func_id_list);
    pthread_mutex_unlock(&context->func_id_mutex);
}

static hy_s32_t _process_server_parse_msg(ipc_link_manager_client_s *ipc_link_client,
        _ipc_process_server_context_s *context)
{
    LOGT("ipc_link_client: %p, context: %p \n", ipc_link_client, context);
    HY_ASSERT_RET_VAL(!ipc_link_client || !context, -1);
    ipc_link_msg_s *ipc_msg = NULL;

    if (0 != ipc_link_read(ipc_link_client->ipc_link_h, &ipc_msg)) {
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
            break;
        case IPC_LINK_MSG_TYPE_CB:
            break;
        case IPC_LINK_MSG_TYPE_CB_ID:
            _handle_ipc_link_msg_cb_id(context, ipc_msg);
            if (ipc_msg) {
                HY_MEM_FREE_P(ipc_msg);
            }
            break;
        default:
            break;
    }

    return 0;
}

static void _ipc_link_manager_accept_cb(void *ipc_link_h, void *args)
{
    LOGT("ipc_link_h: %p, args: %p \n", ipc_link_h, args);
    HY_ASSERT_RET(!ipc_link_h || !args);

    LOGI("ipc process server accept new ipc_link: %p \n", ipc_link_h);

    _ipc_process_server_context_s *context = args;

    HyPipeWrite(context->pipe_h, &ipc_link_h, sizeof(ipc_link_h));
}

static hy_s32_t _read_ipc_link_msg_thread_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    fd_set read_fs = {0};
    struct timeval timeout = {0};
    void *ipc_link_h = NULL;
    _ipc_process_server_context_s *context = args;
    ipc_link_info_s ipc_link_info;
    struct hy_list_head *ipc_link_list = NULL;
    ipc_link_manager_client_s *pos, *n;
    hy_s32_t fd = -1;
    hy_s32_t pipe_read_fd = -1;

    pipe_read_fd = HyPipeReadFdGet(context->pipe_h);

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);

        FD_SET(pipe_read_fd, &read_fs);

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

        if (FD_ISSET(pipe_read_fd, &read_fs)) {
            LOGI("ipc process server send info to client \n");

            HyPipeRead(context->pipe_h, &ipc_link_h, sizeof(ipc_link_h));

            ipc_link_info_get(context->ipc_link_h, &ipc_link_info);

            ipc_link_info_send(ipc_link_h, ipc_link_info.tag, context->pid);
        }

        ipc_link_list = ipc_link_manager_list_get(context->ipc_link_manager_h);
        hy_list_for_each_entry_safe(pos, n, ipc_link_list, entry) {
            fd = ipc_link_get_fd(pos->ipc_link_h);

            if (FD_ISSET(fd, &read_fs)) {
                if (-1 == _process_server_parse_msg(pos, context)) {
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

    _ipc_process_server_context_s *context = *ipc_process_server_h;
    _func_cb_id_s *pos, *n;

    ipc_link_destroy(&context->ipc_link_h);

    context->exit_flag = 1;
    HyThreadDestroy(&context->read_ipc_link_msg_thread_h);
    ipc_link_manager_destroy(&context->ipc_link_manager_h);

    hy_list_for_each_entry_safe(pos, n, &context->func_id_list, entry) {
        hy_list_del(&pos->entry);

        HY_MEM_FREE_P(pos->id);
        HY_MEM_FREE_PP(&pos);
    }

    HyPipeDestroy(&context->pipe_h);

    LOGI("ipc process server destroy, context: %p \n", context);
    HY_MEM_FREE_PP(ipc_process_server_h);
}

void *ipc_process_server_create(HyIpcProcessConfig_s *ipc_process_c)
{
    LOGT("ipc_process_c: %p \n", ipc_process_c);
    HY_ASSERT_RET_VAL(!ipc_process_c, NULL);

    _ipc_process_server_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_process_server_context_s *,
                sizeof(*context));

        HY_MEMCPY(&context->save_config,
                &ipc_process_c->save_config, sizeof(context->save_config));

        context->pid = getpid();
        HY_INIT_LIST_HEAD(&context->func_id_list);
        pthread_mutex_init(&context->func_id_mutex, NULL);

        context->pipe_h = HyPipeCreate_m(HY_PIPE_BLOCK_STATE_NOBLOCK);
        if (!context->pipe_h) {
            LOGE("hy pipe create m failed \n");
            break;
        }

        context->ipc_link_h = ipc_link_create_m(ipc_process_c->ipc_name,
                ipc_process_c->tag, IPC_LINK_TYPE_SERVER, NULL);
        if (!context->ipc_link_h) {
            LOGE("ipc link create m failed \n");
            break;
        }

        ipc_link_manager_config_s ipc_link_manager_c;
        HY_MEMSET(&ipc_link_manager_c, sizeof(ipc_link_manager_c));
        ipc_link_manager_c.save_config.accept_cb    = _ipc_link_manager_accept_cb;
        ipc_link_manager_c.save_config.args         = context;
        ipc_link_manager_c.ipc_link_h               = context->ipc_link_h;
        context->ipc_link_manager_h = ipc_link_manager_create(&ipc_link_manager_c);
        if (!context->ipc_link_manager_h) {
            LOGE("ipc link manager create faeild \n");
            break;
        }

        context->read_ipc_link_msg_thread_h = HyThreadCreate_m("hy_sv_r_ipc_msg",
                _read_ipc_link_msg_thread_cb, context);
        if (!context->read_ipc_link_msg_thread_h) {
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

