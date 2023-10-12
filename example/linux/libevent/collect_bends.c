/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    collect_bends.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    01/09 2023 09:17
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        01/09 2023      create the file
 * 
 *     last modified: 01/09 2023 09:17
 */
#include <stdio.h>
#include <arpa/inet.h>

#include <hy_log/hy_log.h>

#include "hy_mem.h"
#include "hy_string.h"
#include "hy_md5sum.h"
#include "hy_json.h"
#include "hy_time.h"

#include "collect_bends.h"

static char *_protocol_fill_data(hy_u32_t cmd, char *json, hy_s32_t json_len)
{
    unsigned char decrypt[16];
    hy_u32_t len = sizeof(protocol_head_s) + json_len;
    void *buf = HY_MEM_CALLOC_RETURN_VAL(void *, len, NULL);

    protocol_head_s *head = buf;
    head->magic = protocol_magic;
    head->cmd   = cmd;
    head->len   = json_len;

    HY_MEMCPY(head->data, json, json_len);

    HyMd5sum((const hy_u8_t *)json, json_len, decrypt);

    for (hy_u32_t i = 0; i < sizeof(head->crc); i++) {
        head->crc[i] = decrypt[i];
    }

    return buf;
}

static void _dump_info_client(struct bufferevent *bev, protocol_head_s *head,
                              struct evbuffer *evbuf, char *tmp_buf)
{
    hy_s32_t ret;
    char buf[1024] = {0};

    HySocketInfo_s socket_info;
    protocol_get_ip(bev, &socket_info);

    ret = 0;
    ret += snprintf(buf + ret, sizeof(buf) - ret, "socket-->%s:%d, ",
                    socket_info.ip, socket_info.port); 

    // ret += snprintf(buf + ret, sizeof(buf) - ret, "magic: %x, ", head->magic); 
    ret += snprintf(buf + ret, sizeof(buf) - ret, "crc: "); 
    for (hy_s32_t i = 0; i < protocol_crc_len; i++) {
        ret += snprintf(buf + ret, sizeof(buf) - ret, "%02x", head->crc[i]); 
    }
    ret += snprintf(buf + ret, sizeof(buf) - ret, ", "); 
    // ret += snprintf(buf + ret, sizeof(buf) - ret, "cmd: %x, ", head->cmd); 
    // ret += snprintf(buf + ret, sizeof(buf) - ret, "len: %d, ", head->len); 

    ret += snprintf(buf + ret, sizeof(buf) - ret, "json: %s", tmp_buf); 

    LOGI("%s \n", buf);
}

static void _handle_cmd_report_bends(struct bufferevent *bev, char *buf)
{
    void *root = HyJsonCreateFromBuf(buf);

    const char *time_str = HyJsonGetItemStr("err", root, "time");
    const char *sn = HyJsonGetItemStr("err", root, "sn");

    char local_time[64] = {0};
    HyTimeFormatLocalTime(local_time, sizeof(local_time));
    local_time[10] = ' ';

    time_t time_utc = HyTimeFormatTime2UTC(time_str);
    time_t local_time_utc = HyTimeFormatTime2UTC(local_time);

    if (labs(time_utc - local_time_utc) > 3) {
        void *time_root = HyJsonCreate();

        local_time[19] = '\0';

        HyJsonAddStr(time_root, "sn", sn);
        HyJsonAddInt(time_root, "isResetTime", 1);
        HyJsonAddStr(time_root, "time", local_time);

        LOGE("reset time, client time: %s, local time: %s, %ld \n", time_str, local_time, labs(time_utc - local_time_utc));

        char *time_root_str = HyJsonDump(time_root);
        char *str = _protocol_fill_data(0x4, time_root_str, HY_STRLEN(time_root_str));

        // protocol_head_s *head = (protocol_head_s *)str;
        // LOGI("magic: 0x%02x \n", head->magic);
        // LOGI("cmd: 0x%04x \n", head->cmd);
        // LOGI("len: 0x%02x \n", head->len);
        // LOGI("crc: ");
        // for (hy_u32_t i = 0; i < sizeof(head->crc); i++) {
        //     printf("%02x", head->crc[i]);
        // }
        // printf("\n");
        // LOGE("str: %s \n", str + 16);

        // if (0 != bufferevent_write(bev, str, sizeof(protocol_head_s) + HY_STRLEN(time_root_str))) {
        //     LOGE("write data failed \n");
        // }
        HY_MEM_FREE_PP(&time_root_str);
        HY_MEM_FREE_PP(&str);

        HyJsonDestroy(time_root);
    }

    HyJsonDestroyFromBuf(root);
}

void protocol_handle_frame(struct bufferevent *bev, struct evbuffer *evbuf)
{
    hy_s32_t ret;
    char tmp_buf[1024] = {0};
    struct evbuffer_iovec frame;
    protocol_head_s *head;

    ret = evbuffer_peek(evbuf, -1, NULL, &frame, 1);
    if (ret > 0) {
        head = (protocol_head_s *)frame.iov_base;

        if (head->magic == protocol_magic) {
            evbuffer_drain(evbuf, sizeof(*head));
            evbuffer_copyout(evbuf, tmp_buf, head->len);
            evbuffer_drain(evbuf, head->len);

            _dump_info_client(bev, head, evbuf, tmp_buf);

            switch (head->cmd) {
                case 0x2:
                    _handle_cmd_report_bends(bev, tmp_buf);
                    break;
                default:
                    break;
            }

        } else {
            HY_MEMSET(tmp_buf, sizeof(tmp_buf));
            evbuffer_copyout(evbuf, tmp_buf, frame.iov_len);
            evbuffer_drain(evbuf, frame.iov_len);

            LOGI("buf<only display ascii>: %s \n", tmp_buf);
        }
    } else {
        LOGE("evbuffer_peek error \n");
    }
}

void protocol_get_ip(struct bufferevent *bev, HySocketInfo_s *socket_info)
{
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    hy_s32_t fd;

    HY_MEMSET(socket_info, sizeof(*socket_info));

    fd = bufferevent_getfd(bev);
    if (getpeername(fd, (struct sockaddr*)&addr, &addr_len) == 0) {
        if (addr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&addr;
            inet_ntop(AF_INET, &s->sin_addr, socket_info->ip, INET_ADDRSTRLEN);
            socket_info->port = s->sin_port;
        } else if (addr.ss_family == AF_INET6) {
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
            inet_ntop(AF_INET6, &s->sin6_addr, socket_info->ip, INET6_ADDRSTRLEN);
            socket_info->port = s->sin6_port;
        }
    }
}
