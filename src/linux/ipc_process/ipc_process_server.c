/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_server.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/01 2022 16:28
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/01 2022      create the file
 * 
 *     last modified: 21/01 2022 16:28
 */
#include <stdio.h>

#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_thread_mutex.h"

#include "hy_ipc_socket.h"
#include "ipc_process_link.h"
#include "ipc_process_server.h"

typedef struct {
    void                                    *mutex_handle;
    struct hy_list_head                     list;

    ipc_process_link_s                      *link;
    void                                    *accept_thread_handle;
    ipc_process_link_accept_cb_t            accept_cb;

    hy_s32_t                                exit_flag:2;
    hy_s32_t                                reserved;
} _ipc_process_server_s;

static void _accept_cb(void *handle, void *args)
{
    LOGE("handle: %p, args: %p \n", handle, args);

    _ipc_process_server_s *server = args;

    ipc_process_link_s *link = ipc_process_link_create_2(handle);
    if (!link) {
        LOGE("ipc_process_link_create_2 failed \n");
        return;
    }

    HyThreadMutexLock(server->mutex_handle);
    hy_list_add_tail(&link->list, &server->list);
    HyThreadMutexUnLock(server->mutex_handle);
}

static hy_s32_t _accept_loop_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ipc_process_server_s *server = args;
    hy_s32_t ret = 0;

    ret = ipc_process_link_accept(server->link, _accept_cb, server);
    if (0 != ret) {
        LOGE("HyIpcSocketAccept failed \n");
        // @fixme 失败了，要往上传导
    }

    return -1;
}

void ipc_process_server_destroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _ipc_process_server_s *server = *handle;

    ipc_process_link_destroy((void **)&server->link);

    HyThreadDestroy(&server->accept_thread_handle);

    HyThreadMutexLock(server->mutex_handle);
    ipc_process_link_s *pos, *n;
    hy_list_for_each_entry_safe(pos, n, &server->list, list) {
        hy_list_del(&pos->list);

        ipc_process_link_destroy((void **)&pos);
    }
    HyThreadMutexUnLock(server->mutex_handle);

    HyThreadMutexDestroy(&server->mutex_handle);

    LOGI("ipc process link manager destroy, server: %p \n", server);
    HY_MEM_FREE_PP(handle);
}

void *ipc_process_server_create(const char *ipc_name)
{
    LOGT("ipc process server ipc_name: %s \n", ipc_name);
    HY_ASSERT_RET_VAL(!ipc_name, NULL);

    _ipc_process_server_s *server = NULL;

    do {
        server = HY_MEM_MALLOC_BREAK(_ipc_process_server_s *, sizeof(*server));

        HY_INIT_LIST_HEAD(&server->list);

        server->mutex_handle = HyThreadMutexCreate_m();
        if (!server->mutex_handle) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        server->link = ipc_process_link_create(ipc_name,
                IPC_PROCESS_LINK_TYPE_SERVER);
        if (!server->link) {
            LOGE("ipc_process_link_create failed \n");
            break;
        }

        server->accept_thread_handle = HyThreadCreate_m("hy_accept",
                _accept_loop_cb, server);
        if (!server->accept_thread_handle) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        LOGI("ipc process link manager create, server: %p, \n", server);
        return server;
    } while (0);

    LOGE("ipc process link manager create failed \n");
    ipc_process_server_destroy((void **)&server);
    return NULL;
}
