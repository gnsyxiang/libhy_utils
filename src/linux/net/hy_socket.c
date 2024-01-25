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
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>

#include <hy_os_type/hy_assert.h>
#include <hy_os_type/hy_mem.h>

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
hy_s32_t HySocketClientTCPWriteOnceTimeout(const char *ip, hy_u16_t port,
                                           hy_u32_t ms, void *buf, hy_u32_t len)
{
    hy_s32_t socket_fd = -1;
    hy_s32_t ret;

    HY_ASSERT_RET_VAL(!ip || !buf, -1);

    do {
        socket_fd = HySocketCreate(HY_SOCKET_DOMAIN_TCP);
        if (socket_fd < 0) {
            LOGE("HySocketCreate failed \n");
            break;
        }

        if (-1 == HySocketConnectTimeout(socket_fd, ms, ip, port)) {
            LOGE("HySocketConnectTimeout failed \n");
            break;
        }

        ret = write(socket_fd, buf, len);
        if (-1 == ret) {
            LOGES("write failed \n");
            break;
        }

        HySocketDestroy(&socket_fd);

        return ret;
    } while(0);

    if (socket_fd) {
        HySocketDestroy(&socket_fd);
    }

    return -1;
}

hy_s32_t HySocketClientTCPWriteOnce(const char *ip, hy_u16_t port,
                                    const void *buf, hy_u32_t len)
{
    hy_s32_t socket_fd = -1;
    hy_s32_t ret;

    HY_ASSERT_RET_VAL(!ip || !buf, -1);

    do {
        socket_fd = HySocketCreate(HY_SOCKET_DOMAIN_TCP);
        if (socket_fd < 0) {
            LOGE("HySocketCreate failed \n");
            break;
        }

        if (-1 == HySocketConnect(socket_fd, ip, port)) {
            LOGE("HySocketConnect failed \n");
            break;
        }

        ret = write(socket_fd, buf, len);
        if (-1 == ret) {
            LOGES("write failed \n");
            break;
        }

        HySocketDestroy(&socket_fd);

        return ret;
    } while(0);

    if (socket_fd) {
        HySocketDestroy(&socket_fd);
    }

    return -1;
}

hy_s32_t HySocketUnixConnect(hy_s32_t socket_fd, const char *file_path)
{
    HY_ASSERT_RET_VAL(!file_path, -1);
    hy_s32_t ret;
    struct sockaddr_un server_addr;

    HY_MEMSET(&server_addr, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, file_path);

    ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(ret < 0) {
        LOGES("unix socket connect failed \n");
        return -1;
    } else {
        return 0;
    }
}

hy_s32_t HySocketUnixListen(hy_s32_t socket_fd, const char *file_path)
{
    HY_ASSERT_RET_VAL(!file_path, -1);
    hy_s32_t ret;
    struct sockaddr_un addr;

    do {
        unlink(file_path);

        HY_MEMSET(&addr, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, file_path, sizeof(addr.sun_path) - 1);

        ret = bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0) {
            LOGES("unix bind failed \n");
            break;
        }

        ret = listen(socket_fd, 16);
        if (ret != 0) {
            LOGES("unix listen failed \n");
            break;
        }

        return 0;
    } while(0);

    unlink(file_path);

    LOGE("unix socket listen failed \n");
    return -1;
}

hy_s32_t HySocketUnixAccept(hy_s32_t socket_fd, struct sockaddr_un *client_addr)
{
    HY_ASSERT_RET_VAL(!client_addr, -1);
    hy_s32_t new_fd = -1;

    socklen_t addr_len = sizeof(*client_addr);
    new_fd = accept(socket_fd, (struct sockaddr *)client_addr, &addr_len);
    if (-1 == new_fd) {
        LOGES("unix accept failed \n");
    }

    return new_fd;
}

void HySocketUnixDestroy(hy_s32_t *socket_fd, const char *file_path)
{
    if (socket_fd && *socket_fd) {
        LOGI("close socket fd: %d \n", *socket_fd);

        close(*socket_fd);
        *socket_fd = -1;
    }

    if (file_path) {
        unlink(file_path);
    }
}

hy_s32_t HySocketUnixCreate(void)
{
    hy_s32_t socket_fd = -1;

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        LOGES("unix socket failed \n");
    }

    LOGI("unix socket fd: %d \n", socket_fd);
    return socket_fd;
}

hy_s32_t HySocketAccept(hy_s32_t socket_fd, struct sockaddr_in *client_addr)
{
    HY_ASSERT_RET_VAL(!client_addr, -1);
    hy_s32_t new_fd = -1;

    socklen_t addr_len = sizeof(*client_addr);
    new_fd = accept(socket_fd, (struct sockaddr *)client_addr, &addr_len);
    if (-1 == new_fd) {
        LOGES("accept failed \n");
        return -1;
    }

    LOGI("new client socket fd: %d \n", new_fd);
    return new_fd;
}

hy_s32_t HySocketListen(hy_s32_t socket_fd, const char *ip, hy_u16_t port)
{
    hy_s32_t ret;
    struct sockaddr_in addr;
    hy_s32_t flag = 1;

    do {
        ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag,sizeof(flag));
        if (ret == -1){
            LOGES("setsockopt failed \n");
            break;
        }

        HY_MEMSET(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (ip) {
            addr.sin_addr.s_addr = inet_addr(ip);
        } else {
            addr.sin_addr.s_addr = INADDR_ANY;
        }

        ret = bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0) {
            LOGES("bind failed \n");
            break;
        }

        ret = listen(socket_fd, 16);
        if (ret != 0) {
            LOGES("listen failed \n");
            break;
        }

        return 0;
    } while(0);

    LOGE("socket listen failed \n");
    return -1;
}

hy_s32_t HySocketConnect(hy_s32_t socket_fd, const char *ip, const hy_u16_t port)
{
    hy_s32_t ret;
    struct sockaddr_in server_addr;

    HY_ASSERT_RET_VAL(!ip, -1);

    HY_MEMSET(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    ret = connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret < 0) {
        LOGES("socket connect %s:%d failed \n", ip, port);
        return -1;
    } else {
        return 0;
    }
}

hy_s32_t HySocketConnectTimeout(hy_s32_t socket_fd, hy_u32_t ms,
                                const char *ip, const hy_u16_t port)
{
    hy_s32_t ret, flag;
    struct sockaddr_in server_addr;
    struct timeval tv;
    fd_set wset,rset;

    HY_ASSERT_RET_VAL(!ip, -1);

    HY_MEMSET(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    fcntl(socket_fd, F_SETFL, fcntl(socket_fd, F_GETFL, 0) | O_NONBLOCK);

    ret = connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret != 0) {
        if (errno != EINPROGRESS) {
            LOGES("connect failed \n");
        } else {
            tv.tv_sec = 0;
            tv.tv_usec = ms * 1000;

            FD_ZERO(&rset);
            FD_ZERO(&wset);
            FD_SET(socket_fd, &wset);
            FD_SET(socket_fd, &rset);

            flag = select(socket_fd + 1, &rset, &wset, NULL, &tv);
            if (flag < 0) {
                LOGES("select failed \n");
            } else if (flag == 0) {
                ret = -2;
                LOGES("select timeout \n");
            } else if (flag > 0) {
                if (FD_ISSET(socket_fd, &wset) || FD_ISSET(socket_fd, &rset)) {
                    fcntl(socket_fd, F_SETFL, fcntl(socket_fd, F_GETFL, 0) & ~O_NONBLOCK);
                    ret = 0;
                } else {
                    LOGES("select timeout \n");
                }
            }
        }
    }

    return ret;
}

void HySocketDestroy(hy_s32_t *socket_fd)
{
    HY_ASSERT_RET(!socket_fd || *socket_fd < 0);

    LOGD("close socket fd: %d \n", *socket_fd);

    close(*socket_fd);
    *socket_fd = -1;
}

hy_s32_t HySocketCreate(HySocketDomain_e domain)
{
    hy_s32_t socket_fd = -1;

    switch (domain) {
        case HY_SOCKET_DOMAIN_TCP:
            socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd < 0) {
                LOGES("socket failed \n");
                socket_fd = -1;
                break;
            }
            LOGD("tcp socket fd: %d \n", socket_fd);
            break;
        case HY_SOCKET_DOMAIN_UDP:
            socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (socket_fd < 0) {
                LOGES("socket failed \n");
                socket_fd = -1;
                break;
            }
            LOGD("udp socket fd: %d \n", socket_fd);
            break;
        default:
            LOGE("the domain is error \n");
            break;
    }

    return socket_fd;
}
