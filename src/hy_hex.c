/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_hex.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    12/04 2022 10:22
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        12/04 2022      create the file
 * 
 *     last modified: 12/04 2022 10:22
 */
#include <stdio.h>

#include <hy_log/hy_log.h>

#include "hy_hex.h"

void HyHexBit(const void *buf, hy_u32_t len, char *out_buf, hy_u32_t out_len)
{
    unsigned char tmp;
    hy_u32_t ret = 0;
    hy_u32_t cnt;

    for (hy_u32_t i = 0; i < len; i++) {
        tmp = *(unsigned char *)(buf + i);
        if (tmp) {
            ret += snprintf(out_buf + ret, out_len - ret, "(%d, 0x%x)<", tmp, tmp);
            cnt = 8;
            while (cnt--) {
                ret += snprintf(out_buf + ret, out_len - ret, "%d", (tmp >> cnt) & 0x1);
            }
            ret += snprintf(out_buf + ret, out_len - ret, "> ");
        }
    }
}

hy_u32_t HyHex(const void *buf, hy_u32_t len, char *out_buf, hy_u32_t out_len, hy_s32_t flag)
{
    hy_s32_t cnt = 0;
    hy_s32_t ret = 0;
    const hy_u8_t *str = (const hy_u8_t *)buf;

    if (len <= 0) {
        return 0;
    }

    for (hy_u32_t i = 0; i < len; i++) {
        if (flag == 1) {
            if (str[i] == 0x0d || str[i] == 0x0a || str[i] < 32 || str[i] >= 127) {
                ret += snprintf(out_buf + ret, out_len - ret, "%02x[ ]  ", str[i]);
            } else {
                ret += snprintf(out_buf + ret, out_len - ret, "%02x[%c]  ", str[i], str[i]);
            }
        } else {
            ret += snprintf(out_buf + ret, out_len - ret, "%02x  ", str[i]);
        }

        cnt++;
        if (cnt == 16) {
            cnt = 0;
            ret += snprintf(out_buf + ret, out_len - ret, "\r\n");
        }
    }
    ret += snprintf(out_buf + ret, out_len - ret, "\r\n");

    return ret;
}
