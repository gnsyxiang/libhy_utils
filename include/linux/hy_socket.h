/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 08:21
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 08:21
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_SOCKET_H_
#define __LIBHY_UTILS_INCLUDE_HY_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hy_hal/hy_type.h"

#define HY_SOCKET_NAME_LEN_MAX  (32)

typedef enum {
    HY_SOCKET_TYPE_CLIENT,
    HY_SOCKET_TYPE_SERVER,

    HY_SOCKET_TYPE_MAX,
} HySocketType_e;

typedef enum {
    HY_SOCKET_INFO_FD,

    HY_SOCKET_INFO_MAX,
} HySocketInfo_e;

typedef struct {
    char            name[HY_SOCKET_NAME_LEN_MAX];
    HySocketType_e  type:2;
    HySocketType_e  reserved;
} HySocketSaveConfig_s;

typedef struct {
    HySocketSaveConfig_s save_config;
} HySocketConfig_s;

void *HySocketCreate(HySocketConfig_s *config);
void HySocketDestroy(void **handle);

hy_s32_t HySocketConnectServer(void *handle);
void HySocketWaitForConnect(void *handle);

void HySocketRead(void *handle);
void HySocketWrite(void *handle);

void HySocketGetInfo(void *handle);

#ifdef __cplusplus
}
#endif

#endif
