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

/**
 * 本地字节序
 * 不同操作系统可能会采用不同的字节序，所以当通信双方字节序不同时需要考虑字节序转化问题
 *
 * 网络字节序
 * 为了避免在网络通信中引入其他复杂性，网络字节序统一是大端的
 *
 * 序列化与反序列化
 * 序列化会把内存对齐的填充去掉，直接传输数据本身
 * 通过序列化可以使得网络传输得到数据量更少，通过反序列化使得数据接受时格式与原来一致。
 *
 * 本地字节序和网络字节序转换
 * htonl()--"Host to Network Long"
 * ntohl()--"Network to Host Long"
 * htons()--"Host to Network Short"
 * ntohs()--"Network to Host Short" 
 *
 * 哪些数据需要转换
 * 我们只需要将IP网络层需要访问的数据转化为大端模式，这样网络才知道如何传递、转发数据。
 * 而这些数据是指IP地址和端口，IP网络层需要根据这些信息进行转发。
 * 而发送的具体信息，并不被网络层所读取（只是传输），
 * 因此只要保证接受方与发送发使用的字节序相同，就不需要进行转换
 */

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
