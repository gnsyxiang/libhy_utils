/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link_manager.c
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

#include "ipc_link_manager.h"

struct hy_list_head *ipc_link_manager_get_list(void *handle)
{
    LOGT("handle: %p \n", handle);
    HY_ASSERT_RET_VAL(!handle, NULL);

    ipc_link_manager_s *ipc_link_manager = handle;

    pthread_mutex_lock(&ipc_link_manager->mutex);

    return &ipc_link_manager->list;
}

void ipc_link_manager_put_list(void *handle)
{
    LOGT("handle: %p \n", handle);
    HY_ASSERT_RET(!handle);

    ipc_link_manager_s *ipc_link_manager = handle;

    pthread_mutex_unlock(&ipc_link_manager->mutex);
}

static void _link_manager_accept_cb(void *handle, void *args)
{
    LOGT("handle: %p, args: %p \n", handle, args);
    HY_ASSERT_RET(!handle || !args);

    ipc_link_manager_s *ipc_link_manager = args;
    const char *ipc_name = NULL;

    ipc_name = HyIpcSocketGetName(ipc_link_manager->link->ipc_socket_h);

    ipc_link_s *ipc_link = ipc_link_create(ipc_name, NULL,
            IPC_LINK_TYPE_MAX, handle);
    if (!ipc_link) {
        LOGE("ipc link create failed \n");
        return;
    }

    LOGI("ipc link manager accept new ipc link to list, "
            "ipc_socket_h: %p, ipc_link: %p \n", handle, ipc_link);

    pthread_mutex_lock(&ipc_link_manager->mutex);
    hy_list_add_tail(&ipc_link->entry, &ipc_link_manager->list);
    pthread_mutex_unlock(&ipc_link_manager->mutex);

    if (ipc_link_manager->accept_cb) {
        ipc_link_manager->accept_cb(ipc_link, ipc_link_manager->args);
    }
}

static hy_s32_t _link_manager_wait_accept_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    ipc_link_manager_s *ipc_link_manager = args;

    return ipc_link_wait_accept(ipc_link_manager->link,
            _link_manager_accept_cb, ipc_link_manager);
}

void ipc_link_manager_destroy(ipc_link_manager_s **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    ipc_link_manager_s *ipc_link_manager = *handle;
    ipc_link_s *pos, *n;

    ipc_link_destroy(&ipc_link_manager->link);
    HyThreadDestroy(&ipc_link_manager->link_manager_thread_h);

    pthread_mutex_lock(&ipc_link_manager->mutex);
    hy_list_for_each_entry_safe(pos, n, &ipc_link_manager->list, entry) {
        hy_list_del(&pos->entry);

        // pthread_mutex_unlock(&ipc_link_manager->mutex);
        ipc_link_destroy(&pos);
        // pthread_mutex_lock(&ipc_link_manager->mutex);
    }
    pthread_mutex_unlock(&ipc_link_manager->mutex);

    pthread_mutex_destroy(&ipc_link_manager->mutex);

    LOGI("link manager destroy, ipc_link_manager: %p \n", ipc_link_manager);
    HY_MEM_FREE_PP(handle);
}

void *ipc_link_manager_create(const char *name, const char *tag,
        ipc_link_manager_accept_cb_t accept_cb, void *args)
{
    LOGT("name: %s, tag: %s, accept_cb: %p, args: %p \n",
            name, tag, accept_cb, args);
    HY_ASSERT_RET_VAL(!name || !tag || !accept_cb, NULL);

    ipc_link_manager_s *ipc_link_manager = NULL;

    do {
        ipc_link_manager = HY_MEM_MALLOC_BREAK(ipc_link_manager_s *,
                sizeof(*ipc_link_manager));

        HY_INIT_LIST_HEAD(&ipc_link_manager->list);
        pthread_mutex_init(&ipc_link_manager->mutex, NULL);

        ipc_link_manager->accept_cb  = accept_cb;
        ipc_link_manager->args       = args;

        ipc_link_manager->link = ipc_link_create(name, tag,
                IPC_LINK_TYPE_SERVER, NULL);
        if (!ipc_link_manager->link) {
            LOGE("ipc link create failed \n");
            break;
        }

        ipc_link_manager->link_manager_thread_h = HyThreadCreate_m(
                "link_manager_accept",
                _link_manager_wait_accept_cb,
                ipc_link_manager);
        if (!ipc_link_manager->link_manager_thread_h) {
            LOGE("hy thread create failed \n");
            break;
        }

        LOGI("link manager create, ipc_link_manager: %p \n", ipc_link_manager);
        return ipc_link_manager;
    } while (0);

    LOGE("link manager create failed \n");
    ipc_link_manager_destroy(&ipc_link_manager);
    return NULL;
}
