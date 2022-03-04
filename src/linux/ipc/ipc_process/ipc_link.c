/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 10:46
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 10:46
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "ipc_link.h"
#include "hy_ipc_socket.h"

typedef struct {
    char    tag[IPC_LINK_IPC_NAME_LEN_MAX];

    void    *ipc_socket_h;
} _ipc_link_context_s;

hy_s32_t ipc_link_connect(void *ipc_link_h, hy_u32_t timeout_s)
{
    LOGT("ipc_link_h: %p, timeout_s: %d \n", ipc_link_h, timeout_s);
    HY_ASSERT_RET_VAL(!ipc_link_h, -1);

    _ipc_link_context_s *context = ipc_link_h;

    return HyIpcSocketConnect(context->ipc_socket_h, timeout_s);
}

hy_s32_t ipc_link_wait_accept(void *ipc_link_h,
        ipc_link_accept_cb_t accept_cb, void *args)
{
    LOGT("ipc_link_h: %p, accept_cb: %p, args: %p \n",
            ipc_link_h, accept_cb, args);
    HY_ASSERT_RET_VAL(!ipc_link_h || !accept_cb, -1);

    _ipc_link_context_s *context = ipc_link_h;

    return HyIpcSocketAccept(context->ipc_socket_h, accept_cb, args);
}

hy_s32_t ipc_link_info_get(void *ipc_link_h, ipc_link_info_s *ipc_link_info)
{
    LOGT("ipc_link_h: %p, ipc_link_info: %p \n", ipc_link_h, ipc_link_info);
    HY_ASSERT_RET_VAL(!ipc_link_h || !ipc_link_info, -1);

    _ipc_link_context_s *context = ipc_link_h;

    ipc_link_info->fd       = HyIpcSocketGetFD(context->ipc_socket_h);
    ipc_link_info->ipc_name = HyIpcSocketGetName(context->ipc_socket_h);
    ipc_link_info->tag      = context->tag;

    return 0;
}

hy_s32_t ipc_link_info_set(void *ipc_link_h, const char *tag)
{
    LOGT("ipc_link_h: %p, tag: %s \n", ipc_link_h, tag);
    HY_ASSERT_RET_VAL(!ipc_link_h || !tag, -1);

    _ipc_link_context_s *context = ipc_link_h;

    HY_STRNCPY(context->tag, sizeof(context->tag), tag, HY_STRLEN(tag));

    return 0;
}

hy_s32_t ipc_link_read(void *ipc_link_h, ipc_link_msg_s **ipc_link_msg)
{
    LOGT("ipc_link_h: %p, ipc_link_msg: %p, \n", ipc_link_h, ipc_link_msg);
    HY_ASSERT(ipc_link_h);
    HY_ASSERT(ipc_link_msg);

    _ipc_link_context_s *context = ipc_link_h;
    ipc_link_msg_s *ipc_link_msg_tmp = NULL;
    hy_s32_t total_len = 0;

    do {
        if (HY_IPC_SOCKET_CONNECT_STATE_CONNECT
                != HyIpcSocketGetConnectState(context->ipc_socket_h)) {
            LOGE("ipc socket disconnect \n");
            break;
        }

        if (-1 == HyIpcSocketRead(context->ipc_socket_h,
                    &total_len, sizeof(hy_u32_t))) {
            LOGE("ipc socket read failed \n");
            break;
        }

        ipc_link_msg_tmp = HY_MEM_MALLOC_BREAK(ipc_link_msg_s *, total_len);

        if (-1 == HyIpcSocketRead(context->ipc_socket_h,
                    ipc_link_msg_tmp, total_len)) {
            LOGE("ipc socket read failed \n");
            break;
        }

        *ipc_link_msg = ipc_link_msg_tmp;

        return 0;
    } while (0);

    if (ipc_link_msg_tmp) {
        HY_MEM_FREE_PP(&ipc_link_msg_tmp);
    }

    return -1;
}

hy_s32_t ipc_link_write(void *ipc_link_h, ipc_link_msg_s *ipc_link_msg)
{
    LOGT("ipc_link_h: %p, ipc_link_msg: %p \n", ipc_link_h, ipc_link_msg);
    HY_ASSERT(ipc_link_h);
    HY_ASSERT(ipc_link_msg);

    _ipc_link_context_s *context = ipc_link_h;

    hy_s32_t ret = -1;

    do {
        if (HY_IPC_SOCKET_CONNECT_STATE_CONNECT
                != HyIpcSocketGetConnectState(context->ipc_socket_h)) {
            LOGE("hy ipc socket get connect state failed \n");
            break;
        }

        if (-1 == HyIpcSocketWrite(context->ipc_socket_h,
                    &ipc_link_msg->total_len, sizeof(hy_u32_t))) {
            LOGE("ipc socket write failed \n");
            break;
        }

        if (-1 == HyIpcSocketWrite(context->ipc_socket_h,
                    ipc_link_msg, ipc_link_msg->total_len)) {
            LOGE("ipc socket write failed \n");
            break;
        }

        ret = 0;
    } while (0);

    HY_MEM_FREE_P(ipc_link_msg);

    return (ret == 0 ? 0 : -1);
}

void ipc_link_destroy(void **ipc_link_h)
{
    LOGT("&ipc_link_h: %p, ipc_link_h: %p \n", ipc_link_h, *ipc_link_h);
    HY_ASSERT_RET(!ipc_link_h || !*ipc_link_h);

    _ipc_link_context_s *context = *ipc_link_h;

    HyIpcSocketDestroy(&context->ipc_socket_h);

    LOGI("ipc link destroy, context: %p \n", context);
    HY_MEM_FREE_PP(ipc_link_h);
}

void *ipc_link_create(ipc_link_config_s *ipc_link_c)
{
    LOGT("ipc_link_c: %p \n", ipc_link_c);
    HY_ASSERT_RET_VAL(!ipc_link_c, NULL);

    _ipc_link_context_s *context = NULL;
    HyIpcSocketType_e ipc_socket_type[HY_IPC_SOCKET_TYPE_MAX] = {
        HY_IPC_SOCKET_TYPE_CLIENT, HY_IPC_SOCKET_TYPE_SERVER
    };

    do {
        context = HY_MEM_MALLOC_BREAK(_ipc_link_context_s *, sizeof(*context));

        if (ipc_link_c->ipc_socket_h) {
            context->ipc_socket_h = ipc_link_c->ipc_socket_h;
        } else {
            HY_STRNCPY(context->tag, sizeof(context->tag),
                    ipc_link_c->tag, HY_STRLEN(ipc_link_c->tag));

            context->ipc_socket_h = HyIpcSocketCreate_m(ipc_link_c->ipc_name,
                    ipc_socket_type[ipc_link_c->type]);
            if (!context->ipc_socket_h) {
                LOGE("hy ipc socket create failed \n");
                break;
            }
        }

        LOGI("ipc link create, context: %p \n", context);
        return context;
    } while (0);

    LOGE("ipc link create failed \n");
    ipc_link_destroy((void **)&context);
    return NULL;
}

