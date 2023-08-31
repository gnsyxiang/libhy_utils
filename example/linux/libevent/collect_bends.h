/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    collect_bends.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    01/09 2023 09:18
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        01/09 2023      create the file
 * 
 *     last modified: 01/09 2023 09:18
 */
#ifndef __LIBHY_UTILS_INCLUDE_COLLECT_BENDS_H_
#define __LIBHY_UTILS_INCLUDE_COLLECT_BENDS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

#include "hy_type.h"

#define protocol_magic (0x55aa)
#define protocol_crc_len (6)

typedef struct {
    hy_s16_t    magic;
    hy_u8_t     crc[protocol_crc_len];
    hy_u32_t    cmd;
    hy_u32_t    len;

    char        data[0];
} protocol_head_s;

void protocol_handle_frame(struct bufferevent *bev, struct evbuffer *evbuf);

#ifdef __cplusplus
}
#endif

#endif

