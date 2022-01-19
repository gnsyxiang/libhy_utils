/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket_client.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 16:38
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 16:38
 */
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"

#include "hy_socket_client.h"

hy_s32_t hy_client_connect(hy_socket_context_s *context, hy_u32_t timeout_s)
{
    LOGT("handle: %p, timeout_s: %d \n", context, timeout_s);
    HY_ASSERT_VAL_RET_VAL(!context, -1);

    hy_socket_s *socket = context->socket;
    hy_u32_t addr_len = 0;
    hy_s32_t ret = 0;
    hy_u32_t time_cnt = 0;
    struct sockaddr_un addr;

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket->name);

    addr_len = strlen(socket->name) + offsetof(struct sockaddr_un, sun_path);

    do {
        ret = connect(socket->fd, (const struct sockaddr *)&addr, addr_len);
        if (ret < 0) {
            LOGES("connect failed, fd: %d, name: %s, sleep and connect \n",
                    socket->fd, socket->name);

            sleep(1);
        }
    } while ((ret < 0) && (++time_cnt < timeout_s));

    if (time_cnt < timeout_s) {
        return 0;
    } else {
        return -1;
    }
}

void hy_client_destroy(hy_socket_context_s **context_pp)
{
    LOGT("handle: %p, *handle: %p \n", context_pp, *context_pp);
    HY_ASSERT_VAL_RET(!context_pp || !*context_pp);

    hy_socket_context_s *context = *context_pp;
    hy_socket_s *socket = context->socket;

    close(socket->fd);

    LOGI("socket client destroy, handle: %p, fd: %d \n",
            context->socket, context->socket->fd);
    hy_socket_socket_destroy(&socket);
}

hy_s32_t hy_client_create(hy_socket_context_s *context)
{
    LOGT("handle: %p \n", context);
    HY_ASSERT_VAL_RET_VAL(!context, -1);

    hy_s32_t fd;
    HySocketSaveConfig_s *save_config = &context->save_config;

    do {
        context->socket = hy_socket_socket_create(save_config->name);
        if (!context->socket) {
            LOGE("socket create failed \n");
            break;
        }

        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd < 0) {
            LOGES("socket failed \n");
            break;
        }

        context->socket->fd = fd;

        LOGI("socket client create, handle: %p, fd: %d \n", context->socket, fd);
        return 0;
    } while (0);

    return -1;
}
