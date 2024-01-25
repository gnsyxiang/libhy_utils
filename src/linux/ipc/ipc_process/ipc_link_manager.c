/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link_manager.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 14:17
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 14:17
 */
#include <stdio.h>

#include <hy_os_type/hy_assert.h>
#include <hy_os_type/hy_mem.h>
#include <hy_os_type/hy_string.h>

#include <hy_os/hy_thread.h>

#include "ipc_link_manager.h"
#include "ipc_link.h"

typedef struct {
    ipc_link_manager_save_config_s  save_config;

    struct hy_list_head             list;
    pthread_mutex_t                 list_mutex;

    void                            *ipc_link_h;
    HyThread_s                      *accept_thread_h;
} _ipc_link_manager_context_s;

struct hy_list_head *ipc_link_manager_list_get(void *ipc_link_manager_h)
{
    LOGT("ipc_link_manager_h: %p \n", ipc_link_manager_h);
    HY_ASSERT_RET_VAL(!ipc_link_manager_h, NULL);

    _ipc_link_manager_context_s *context = ipc_link_manager_h;

    pthread_mutex_lock(&context->list_mutex);

    return &context->list;
}

void ipc_link_manager_list_put(void *ipc_link_manager_h)
{
    LOGT("ipc_link_manager_h: %p \n", ipc_link_manager_h);
    HY_ASSERT_RET(!ipc_link_manager_h);

    _ipc_link_manager_context_s *context = ipc_link_manager_h;

    pthread_mutex_unlock(&context->list_mutex);
}

static void _ipc_link_manager_accept_cb(void *ipc_socket_h, void *args)
{
    LOGE("ipc_socket_h: %p, args: %p \n", ipc_socket_h, args);
    HY_ASSERT_RET(!ipc_socket_h);

    _ipc_link_manager_context_s *context = args;
    ipc_link_manager_save_config_s *save_config = &context->save_config;
    ipc_link_manager_list_s *ipc_link_list = NULL;

    do {
        ipc_link_list = HY_MEM_MALLOC_BREAK(ipc_link_manager_list_s *,
                sizeof(*ipc_link_list));

        ipc_link_list->ipc_link_h = ipc_link_create_m(NULL, NULL,
                IPC_LINK_TYPE_MAX, ipc_socket_h);
        if (!ipc_link_list->ipc_link_h) {
            LOGE("ipc link create m failed \n");
            break;
        }

        LOGI("ipc link manager add new ipc link to list \n");

        pthread_mutex_lock(&context->list_mutex);
        hy_list_add_tail(&ipc_link_list->entry, &context->list);
        pthread_mutex_unlock(&context->list_mutex);

        if (save_config->accept_cb) {
            save_config->accept_cb(ipc_link_list->ipc_link_h, save_config->args);
        }

        return ;
    } while (0);

    LOGI("ipc link manager add new ipc link to list failed \n");

    if (ipc_link_list) {
        if (ipc_link_list->ipc_link_h) {
            ipc_link_destroy(&ipc_link_list->ipc_link_h);
        }
        HY_MEM_FREE_PP(&ipc_link_list);
    }
}

static hy_s32_t _ipc_link_manager_accept_thread_cb(void *args)
{
    LOGT("args: %p \n", args);
    HY_ASSERT_RET_VAL(!args, -1);

    _ipc_link_manager_context_s *context = args;

    return ipc_link_wait_accept(context->ipc_link_h,
            _ipc_link_manager_accept_cb, context);
}

void ipc_link_manager_destroy(void **ipc_link_manager_h)
{
    LOGT("&ipc_link_manager_h: %p, ipc_link_manager_h: %p \n",
            ipc_link_manager_h, *ipc_link_manager_h);
    HY_ASSERT_RET(!ipc_link_manager_h || !*ipc_link_manager_h);

    _ipc_link_manager_context_s *context = *ipc_link_manager_h;
    ipc_link_manager_list_s *pos, *n;

    HyThreadDestroy(&context->accept_thread_h);

    pthread_mutex_lock(&context->list_mutex);
    hy_list_for_each_entry_safe(pos, n, &context->list, entry) {
        hy_list_del(&pos->entry);

        ipc_link_destroy(&pos->ipc_link_h);
        HY_MEM_FREE_PP(&pos);
    }
    pthread_mutex_unlock(&context->list_mutex);

    pthread_mutex_destroy(&context->list_mutex);

    LOGI("ipc link manager destroy, context: %p \n", context);
    HY_MEM_FREE_PP(ipc_link_manager_h);
}

void *ipc_link_manager_create(ipc_link_manager_config_s *ipc_link_manager_c)
{
    LOGT("ipc_link_manager_c: %p \n", ipc_link_manager_c);
    HY_ASSERT_RET_VAL(!ipc_link_manager_c, NULL);

    _ipc_link_manager_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_link_manager_context_s *,
                sizeof(*context));

        HY_MEMCPY(&context->save_config, &ipc_link_manager_c->save_config,
                sizeof(context->save_config));

        HY_INIT_LIST_HEAD(&context->list);
        pthread_mutex_init(&context->list_mutex, NULL);

        context->ipc_link_h = ipc_link_manager_c->ipc_link_h;

        context->accept_thread_h = HyThreadCreate_m("HYILM_accept",
                _ipc_link_manager_accept_thread_cb, context);
        if (!context->accept_thread_h) {
            LOGE("hy thread create m failed \n");
            break;
        }

        LOGI("ipc link manager create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc link manager create failed \n");
    ipc_link_manager_destroy((void **)&context);
    return NULL;
}

