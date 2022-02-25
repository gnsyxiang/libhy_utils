/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link_server.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/02 2022 16:34
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/02 2022      create the file
 * 
 *     last modified: 21/02 2022 16:34
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_string.h"

#include "ipc_link_server.h"

typedef struct {
    ipc_link_s                  *link;

    ipc_link_server_accept_cb   accept;
    void                        *args;
    void                        *accept_thread_handle;

    struct hy_list_head         list;
    pthread_mutex_t             mutex;
} _ipc_link_server_s;

void ipc_link_server_detect_fd(void *handle, fd_set *read_fs,
        ipc_link_server_detect_fd_cb detect_fd_cb, void *args)
{
    LOGT("handle: %p, read_fs: %p, detect_fd_cb: %p, void *args: %p \n",
            handle, read_fs, detect_fd_cb, args);
    HY_ASSERT_RET(!handle || !read_fs || !detect_fd_cb);

    _ipc_link_server_s *server_link = handle;
    ipc_link_msg_s *ipc_msg = NULL;
    hy_s32_t fd = 0;
    hy_s32_t ret = 0;
    ipc_link_s *pos, *n;

    pthread_mutex_lock(&server_link->mutex);

    hy_list_for_each_entry_safe(pos, n, &server_link->list, entry) {
        fd = HyIpcSocketGetFD(pos->ipc_socket_handle);

        if (FD_ISSET(fd, read_fs)) {
            if (0 != ipc_link_read(pos, &ipc_msg)) {
                LOGE("ipc_link_read failed \n");
                continue;
            }

            switch (ipc_msg->type) {
                case IPC_LINK_MSG_TYPE_RETURN:
                    LOGE("--------haha----return \n");
                    break;
                case IPC_LINK_MSG_TYPE_CB:
                    LOGE("--------haha----cb \n");
                    break;
                case IPC_LINK_MSG_TYPE_INFO:
                    LOGE("--------haha----info \n");
                    break;
                default:
                    break;
            }

            // 处理数据，回调
            ret = detect_fd_cb(args);
            if (0 != ret) {
                LOGE("\n");
            }
        }
    }

    pthread_mutex_unlock(&server_link->mutex);
}

void ipc_link_server_set_fd(void *handle, fd_set *read_fs)
{
    LOGT("handle: %p, read_fs: %p \n", handle, read_fs);
    HY_ASSERT_RET(!handle || !read_fs);

    ipc_link_s *pos;
    _ipc_link_server_s *server_link = handle;

    pthread_mutex_lock(&server_link->mutex);

    hy_list_for_each_entry(pos, &server_link->list, entry) {
        FD_SET(HyIpcSocketGetFD(pos->ipc_socket_handle), read_fs);
    }

    pthread_mutex_unlock(&server_link->mutex);
}

static void _ipc_link_accept_cb(void *handle, void *args)
{
    LOGT("handle: %p, args: %p \n", handle, args);
    HY_ASSERT_RET(!handle || !args);

    _ipc_link_server_s *server_link = args;
    const char *ipc_name = NULL;

    ipc_name = HyIpcSocketGetName(server_link->link->ipc_socket_handle);

    ipc_link_s *client_link = ipc_link_create(ipc_name, NULL, IPC_LINK_TYPE_MAX, handle);
    if (!client_link) {
        LOGE("ipc_link_create failed \n");
        return;
    }

    LOGI("add new client link to list, client_link: %p \n", client_link);

    pthread_mutex_lock(&server_link->mutex);
    hy_list_add_tail(&client_link->entry, &server_link->list);
    pthread_mutex_unlock(&server_link->mutex);

    if (server_link->accept) {
        server_link->accept(client_link, server_link->args);
    }
}

static hy_s32_t _ipc_link_server_accpet_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ipc_link_server_s *server_link = args;

    return ipc_link_wait_accept(server_link->link,
            _ipc_link_accept_cb, server_link);
}

void ipc_link_server_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_link_server_s *server_link = *handle;
    ipc_link_s *pos, *n;

    HyThreadDestroy(&server_link->accept_thread_handle);

    pthread_mutex_lock(&server_link->mutex);
    hy_list_for_each_entry_safe(pos, n, &server_link->list, entry) {
        hy_list_del(&pos->entry);

        // pthread_mutex_unlock(&server_link->mutex);
        ipc_link_destroy(&pos);
        // pthread_mutex_lock(&server_link->mutex);
    }
    pthread_mutex_unlock(&server_link->mutex);

    pthread_mutex_destroy(&server_link->mutex);
    ipc_link_destroy(&server_link->link);

    LOGI("ipc link server destroy, server_link: %p \n", server_link);
    HY_MEM_FREE_PP(handle);
}

void *ipc_link_server_create(const char *name, const char *tag,
        ipc_link_server_accept_cb accept, void *args)
{
    LOGT("name: %s, tag: %s, accept: %p, args: %p \n",
            name, tag, accept, args);
    HY_ASSERT_RET_VAL(!name || !tag || !accept, NULL);

    _ipc_link_server_s *server_link = NULL;

    do {
        server_link = HY_MEM_MALLOC_BREAK(_ipc_link_server_s *,
                sizeof(*server_link));

        HY_INIT_LIST_HEAD(&server_link->list);
        pthread_mutex_init(&server_link->mutex, NULL);

        server_link->accept = accept;
        server_link->args   = args;

        server_link->link = ipc_link_create(name, tag,
                IPC_LINK_TYPE_SERVER, NULL);
        if (!server_link->link) {
            LOGE("ipc_link_create failed \n");
            break;
        }

        HyThreadConfig_s thread_config;
        HyThreadSaveConfig_s *save_config = &thread_config.save_config;
        HY_MEMSET(&thread_config, sizeof(thread_config));
        HY_STRNCPY(save_config->name, HY_THREAD_NAME_LEN_MAX,
                "link_accpet", HY_STRLEN("link_accpet"));
        save_config->destroy_flag   = HY_THREAD_DESTROY_FORCE;
        save_config->thread_loop_cb = _ipc_link_server_accpet_cb;
        save_config->args           = server_link;
        server_link->accept_thread_handle = HyThreadCreate(&thread_config);
        if (!server_link->accept_thread_handle) {
            LOGE("HyThreadCreate failed \n");
            break;
        }

        LOGI("ipc link server create, server_link: %p \n", server_link);
        return server_link;
    } while (0);

    LOGE("ipc link server create failed \n");
    ipc_link_server_destroy((void **)&server_link);
    return NULL;
}
