/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    05/05 2023 10:30
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        05/05 2023      create the file
 * 
 *     last modified: 05/05 2023 10:30
 */
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "hy_assert.h"

#include "hy_socket.h"

hy_s32_t HySocketClientWriteOnce(const char *ip, hy_u16_t port,
                                 void *buf, hy_u32_t len)
{
    HY_ASSERT_RET_VAL(!ip || !buf, -1);
    hy_s32_t socket_fd;
    hy_s32_t ret;
    struct sockaddr_in addr;

    do {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            LOGES("socket failed \n");
            break;
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip);

        ret = connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
        if(ret < 0) {
            LOGES("connect failed \n");
            break;
        }

        ret = write(socket_fd, buf, len);
        if (-1 == ret) {
            LOGES("write failed \n");
            break;
        }

        return ret;
    } while(0);

    if (socket_fd) {
        close(socket_fd);
    }

    return -1;
}
