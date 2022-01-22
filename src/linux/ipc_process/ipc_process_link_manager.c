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
#include "hy_hal/hy_thread_mutex.h"

#include "hy_ipc_socket.h"
#include "ipc_process_link.h"
#include "ipc_process_link_manager.h"

static void _accept_cb(void *handle, void *args)
{
    LOGT("handle: %p, args: %p \n", handle, args);

    ipc_process_link_manager_s *link_manager = args;

    ipc_process_link_s *link = ipc_process_link_create_2(handle);
    if (!link) {
        LOGE("ipc_process_link_create_2 failed \n");
        return;
    }

    HyThreadMutexLock(link_manager->mutex_handle);
    hy_list_add_tail(&link->list, &link_manager->list);
    HyThreadMutexUnLock(link_manager->mutex_handle);
}

static hy_s32_t _accept_loop_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    ipc_process_link_manager_s *link_manager = args;
    hy_s32_t ret = 0;

    ret = ipc_process_link_accept(link_manager->link, _accept_cb, link_manager);
    if (0 != ret) {
        LOGE("HyIpcSocketAccept failed \n");
        // @fixme 失败了，要往上传导
    }

    return -1;
}

void ipc_process_link_manager_destroy(ipc_process_link_manager_s **link_manager_pp)
{
    LOGT("&link_manager: %p, link_manager: %p \n",
            link_manager_pp, *link_manager_pp);
    HY_ASSERT_RET(!link_manager_pp || !*link_manager_pp);

    ipc_process_link_manager_s *link_manager = *link_manager_pp;

    ipc_process_link_destroy(&link_manager->link);

    HyThreadDestroy(&link_manager->accept_thread_handle);

    HyThreadMutexLock(link_manager->mutex_handle);
    ipc_process_link_s *pos, *n;
    hy_list_for_each_entry_safe(pos, n, &link_manager->list, list) {
        hy_list_del(&pos->list);

        ipc_process_link_destroy(&pos);
    }
    HyThreadMutexUnLock(link_manager->mutex_handle);

    HyThreadMutexDestroy(&link_manager->mutex_handle);

    LOGI("ipc process link manager destroy, link_manager: %p \n", link_manager);
    HY_MEM_FREE_PP(link_manager_pp);
}

void *ipc_process_link_manager_create(const char *ipc_name)
{
    ipc_process_link_manager_s *link_manager = NULL;

    do {
        link_manager = HY_MEM_MALLOC_BREAK(ipc_process_link_manager_s *,
                sizeof(*link_manager));

        HY_INIT_LIST_HEAD(&link_manager->list);

        link_manager->mutex_handle = HyThreadMutexCreate_m();
        if (!link_manager->mutex_handle) {
            LOGE("HyThreadMutexCreate_m failed \n");
            break;
        }

        link_manager->link = ipc_process_link_create(ipc_name, IPC_PROCESS_LINK_TYPE_SERVER);
        if (!link_manager->link) {
            LOGE("ipc_process_link_create failed \n");
            break;
        }

        link_manager->accept_thread_handle = HyThreadCreate_m("hy_accept",
                _accept_loop_cb, link_manager);
        if (!link_manager->accept_thread_handle) {
            LOGE("HyThreadCreate_m failed \n");
            break;
        }

        LOGI("ipc process link manager create, link_manager: %p, \n",
                link_manager);
        return link_manager;
    } while (0);

    LOGI("ipc process link manager create failed \n");
    ipc_process_link_manager_destroy(&link_manager);
    return NULL;
}
