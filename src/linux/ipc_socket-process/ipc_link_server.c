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

#include "hy_ipc_socket.h"
#include "ipc_link_server.h"

static void _server_accept_cb(void *handle, void *args)
{
    LOGE("------handle: %p \n", handle);

    free(handle);
}

static hy_s32_t _server_link_accept_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    ipc_link_server_s *server_link = args;

    return HyIpcSocketAccept(server_link->link.ipc_socket_handle,
            _server_accept_cb, server_link);
}

void ipc_link_server_destroy(ipc_link_server_s **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);
    ipc_link_server_s *server_link = *handle;

    HyThreadDestroy(&server_link->accept_thread_handle);

    HyIpcSocketDestroy(&server_link->link.ipc_socket_handle);

    LOGI("ipc link server destroy \n");
    HY_MEM_FREE_PP(handle);
}

void *ipc_link_server_create(const char *name, const char *tag)
{
    LOGT("name: %s, tag: %s \n", name, tag);
    HY_ASSERT_RET_VAL(!name || !tag, NULL);

    ipc_link_server_s *server_link = NULL;

    do {
        server_link = HY_MEM_MALLOC_BREAK(ipc_link_server_s *, sizeof(*server_link));

        server_link->link.ipc_socket_handle = HyIpcSocketCreate_m(name,
                HY_IPC_SOCKET_TYPE_SERVER);
        if (!server_link->link.ipc_socket_handle) {
            LOGE("HyIpcSocketCreate_m failed \n");
            break;
        }

        HyThreadConfig_s thread_config;
        HyThreadSaveConfig_s *save_config = &thread_config.save_config;
        HY_MEMSET(&thread_config, sizeof(thread_config));
        HY_STRNCPY(save_config->name, HY_THREAD_NAME_LEN_MAX,
                "link_accpet", HY_STRLEN("link_accpet"));
        save_config->destroy_flag   = HY_THREAD_DESTROY_FORCE;
        save_config->thread_loop_cb = _server_link_accept_cb;
        save_config->args           = server_link;
        server_link->accept_thread_handle = HyThreadCreate(&thread_config);
        if (!server_link->accept_thread_handle) {
            LOGE("HyThreadCreate failed \n");
            break;
        }

        LOGI("ipc link server create, link: %p \n", link);
        return server_link;
    } while (0);

    LOGE("ipc link server create failed \n");
    ipc_link_server_destroy(&server_link);
    return NULL;
}
