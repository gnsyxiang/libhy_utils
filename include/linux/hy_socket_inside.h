/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket_inside.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 09:17
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 09:17
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_SOCKET_INSIDE_H_
#define __LIBHY_UTILS_INCLUDE_HY_SOCKET_INSIDE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include "hy_socket.h"

typedef struct {
    hy_s32_t                fd;
    pthread_mutex_t         mutex;
} hy_socket_s;

typedef struct {
    HySocketSaveConfig_s    save_config;
    hy_socket_s             *socket;
} hy_socket_context_s;

hy_socket_s *hy_socket_socket_create(void);
void hy_socket_socket_destroy(hy_socket_s **socket);

#ifdef __cplusplus
}
#endif

#endif
