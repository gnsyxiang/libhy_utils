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

#include "hy_type.h"
#include "hy_compile.h"

#include "hy_hex.h"

HY_WEAK void HyHex(const void *_buf, size_t len, hy_s32_t flag)
{
    hy_s32_t cnt = 0;
    const hy_u8_t *buf = (const hy_u8_t *)_buf;

    if (len <= 0) {
        return;
    }

    for (size_t i = 0; i < len; i++) {
        if (flag == 1) {
            if (buf[i] == 0x0d || buf[i] == 0x0a || buf[i] < 32 || buf[i] >= 127) {
                printf("%02x[ ]  ", buf[i]);
            } else {
                printf("%02x[%c]  ", buf[i], buf[i]);
            }
        } else {
            printf("%02x ", buf[i]);
        }

        cnt++;
        if (cnt == 16) {
            cnt = 0;
            printf("\r\n");
        }
    }
    printf("\r\n");
}
