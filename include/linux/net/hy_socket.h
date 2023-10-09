/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    05/05 2023 10:31
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        05/05 2023      create the file
 * 
 *     last modified: 05/05 2023 10:31
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_SOCKET_H_
#define __LIBHY_UTILS_INCLUDE_HY_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <arpa/inet.h>
#include <sys/un.h>

#include <hy_log/hy_type.h>

/**
 * @brief domain类型
 */
typedef enum {
    HY_SOCKET_DOMAIN_TCP,                   ///< 创建面向连接的TCP
    HY_SOCKET_DOMAIN_UDP,                   ///< 创建流式的UDP
} HySocketDomain_e;

#define HY_SOCKET_IP_LEN_MAX    (46)        ///< IPv4的长度是16，IPv6的长度是46

/**
 * @brief ip地址和端口号
 */
typedef struct {
    char        ip[HY_SOCKET_IP_LEN_MAX];   ///< ip地址
    hy_u16_t    port;                       ///< 端口号
} HySocketInfo_s;

/**
 * @brief 根据domain创建指定类型的socket
 *
 * @param domain socket类型，详见HySocketDomain_e
 * @return 成功返回大于0的值，失败返回-1
 */
hy_s32_t HySocketCreate(HySocketDomain_e domain);

/**
 * @brief 销毁socket
 *
 * @param socket_fd 需要销毁的socket fd
 */
void HySocketDestroy(hy_s32_t *socket_fd);

/**
 * @brief 连接服务端
 *
 * @param socket_fd fd
 * @param ip ip地址
 * @param port 端口号
 * @return 成功啊返回0，失败返回-1
 */
hy_s32_t HySocketConnect(hy_s32_t socket_fd, const char *ip, const hy_u16_t port);

/**
 * @brief 服务端监听连接
 *
 * @param socket_fd fd
 * @param ip ip地址
 * @param port 端口号
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HySocketListen(hy_s32_t socket_fd, const char *ip, hy_u16_t port);

/**
 * @brief 服务端接收连接
 *
 * @param socket_fd fd
 * @param client_addr 客户端地址
 * @return 成功返回客户端fd，失败返回-1
 */
hy_s32_t HySocketAccept(hy_s32_t socket_fd, struct sockaddr_in *client_addr);

/**
 * @brief 创建UNIX本地套接字
 *
 * @return 成功返回大于0，失败返回-1
 */
hy_s32_t HySocketUnixCreate(void);

/**
 * @brief 销毁UNIX本地套接字
 *
 * @param socket_fd fd
 * @param file_path 文件路径
 */
void HySocketUnixDestroy(hy_s32_t *socket_fd, const char *file_path);

/**
 * @brief 本地socket连接服务端
 *
 * @param socket_fd fd
 * @param file_path 文件路径
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HySocketUnixConnect(hy_s32_t socket_fd, const char *file_path);

/**
 * @brief 本地socket服务端监听连接
 *
 * @param socket_fd fd
 * @param file_path 文件路径
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HySocketUnixListen(hy_s32_t socket_fd, const char *file_path);

/**
 * @brief 本地socket服务端接收连接
 *
 * @param socket_fd fd
 * @param client_addr 客户端地址
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HySocketUnixAccept(hy_s32_t socket_fd, struct sockaddr_un *client_addr);

/**
 * @brief TCP短连接发送数据
 *
 * @param ip ip地址
 * @param port 端口号
 * @param buf 发送数据的地址
 * @param len 数据的长度
 * @return 成功返回数据的长度，失败返回-1
 */
hy_s32_t HySocketClientTCPWriteOnce(const char *ip, hy_u16_t port,
                                    void *buf, hy_u32_t len);

#ifdef __cplusplus
}
#endif

#endif

