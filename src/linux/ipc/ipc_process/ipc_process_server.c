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
    HyIpcProcessSaveConfig_s        save_config;// 必须放在前面，用于强制类型转换

    pid_t                           pid;
    void                            *ipc_link_h;
    void                            *ipc_link_manager_h;

    void                            *pipe_h;
    void                            *read_ipc_link_msg_thread_h;
    hy_s32_t                        exit_flag;
} _ipc_process_server_context_s;

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
    hy_s32_t pipe_read_fd = -1;

    pipe_read_fd = HyPipeReadFdGet(context->pipe_h);

    while (!context->exit_flag) {
        FD_ZERO(&read_fs);
        FD_SET(pipe_read_fd, &read_fs);

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
    }

    return -1;
}

void ipc_process_server_destroy(void **ipc_process_server_h)
{
    LOGT("&ipc_process_server_h: %p, ipc_process_server_h: %p \n",
            ipc_process_server_h, *ipc_process_server_h);
    HY_ASSERT_RET(!ipc_process_server_h || !*ipc_process_server_h);

    _ipc_process_server_context_s *context = *ipc_process_server_h;

    ipc_link_destroy(&context->ipc_link_h);

    ipc_link_manager_destroy(&context->ipc_link_manager_h);

    context->exit_flag = 1;
    HyThreadDestroy(&context->read_ipc_link_msg_thread_h);

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

