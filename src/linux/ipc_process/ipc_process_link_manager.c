/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_process_link_manager.c
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

#include "hy_ipc_socket.h"

#include "ipc_process_link_manager.h"

static void _accept_cb(void *handle, void *args)
{
    LOGD("handle: %p, args: %p \n", handle, args);
}

static hy_s32_t _accept_loop_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    ipc_process_link_manager_s *link_manager = args;

    return HyIpcSocketAccept(link_manager->ipc_socket_handle, _accept_cb, args);
}

void ipc_process_link_manager_destroy(ipc_process_link_manager_s **link_manager_pp)
{
    LOGT("&link_manager: %p, link_manager: %p \n",
            link_manager_pp, *link_manager_pp);
    HY_ASSERT_RET(!link_manager_pp || !*link_manager_pp);

    ipc_process_link_manager_s *link_manager = *link_manager_pp;

    HyIpcSocketDestroy(&link_manager->ipc_socket_handle);

    HyThreadDestroy(&link_manager->accept_thread_handle);

    LOGI("ipc process link manager destroy, link_manager: %p, "
            "ipc_socket_handle: %p, accept_thread_handle: %p \n",
            link_manager, link_manager->ipc_socket_handle,
            link_manager->accept_thread_handle);

    HY_MEM_FREE_PP(link_manager_pp);
}

void *ipc_process_link_manager_create(const char *ipc_name)
{
    ipc_process_link_manager_s *link_manager = NULL;

    do {
        link_manager = HY_MEM_MALLOC_BREAK(ipc_process_link_manager_s *,
                sizeof(*link_manager));

        link_manager->ipc_socket_handle = HyIpcSocketCreate_m(ipc_name,
                HY_IPC_SOCKET_TYPE_SERVER);
        if (!link_manager->ipc_socket_handle) {
            LOGE("HyIpcSocketCreate_m failed \n");
            break;
        }

        link_manager->accept_thread_handle = HyThreadCreate_m("hy_accept",
                _accept_loop_cb, link_manager);
        if (!link_manager->accept_thread_handle) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        LOGI("ipc process link manager create, link_manager: %p, "
                "ipc_socket_handle: %p, accept_thread_handle: %p \n",
                link_manager, link_manager->ipc_socket_handle,
                link_manager->accept_thread_handle);
        return link_manager;
    } while (0);

    LOGI("ipc process link manager create failed \n");
    ipc_process_link_manager_destroy(&link_manager);
    return NULL;
}
