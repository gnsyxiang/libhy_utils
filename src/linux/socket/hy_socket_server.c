/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket_server.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 09:34
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 09:34
 */
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"

#include "hy_socket_server.h"

void hy_server_destroy(hy_socket_context_s **context)
{

}

hy_s32_t hy_server_create(hy_socket_context_s *context)
{
    hy_s32_t fd;
    struct sockaddr_un addr;
    hy_u32_t addr_len;
    HySocketSaveConfig_s *save_config = &context->save_config;

    HY_ASSERT_VAL_RET_VAL(!context, -1);

    do {
        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd < 0) {
            LOGES("socket failed \n");
            break;
        }

        unlink(save_config->name);

        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, save_config->name);

        addr_len = strlen(save_config->name)
            + offsetof(struct sockaddr_un, sun_path);
        if (bind(fd, (const struct sockaddr *)&addr, addr_len) < 0) {
            LOGES("bind failed \n");
            break;
        }

        return 0;
    } while (0);

    return -1;
}
