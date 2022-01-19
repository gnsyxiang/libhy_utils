/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_utils.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 14:12
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 14:12
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"

#include "hy_utils.h"

void HyUtilsIpStr2Int(const char *ip_str, uint32_t *ip_num)
{
    HY_ASSERT_RET(!ip_str);

    int num[IP_INT_DOT_LEN];

	sscanf(ip_str, "%d.%d.%d.%d", &num[0], &num[1], &num[2], &num[3]);

    *ip_num  = 0;
    for (int i = 0; i < IP_INT_DOT_LEN; i++) {
        *ip_num += (num[i] & 0xff) << (8 * (3 - i));
    }
}

void HyUtilsIpInt2Str(uint32_t ip_num, char *ip_str, size_t ip_str_len)
{
    HY_ASSERT_RET(!ip_str);

    uint8_t *tmp = (uint8_t *)&ip_num;
    size_t len = 0;

    memset(ip_str, '\0', ip_str_len);

    for (int i = 0; i < IP_INT_DOT_LEN; i++) {
        len += snprintf(ip_str + len, ip_str_len - len,
                "%d.", *(tmp + IP_INT_DOT_LEN - 1 - i));
    }
    ip_str[len - 1] = '\0';
}

void HyUtilsHex2Int2Str(char *addr, size_t addr_len, char *str, size_t str_len)
{
    size_t len = 0;
    for (size_t i = 0; i < addr_len; i++) {
        // LOGD("addr: %02x, %d \n", addr[i], (uint8_t)addr[i]);
        len += snprintf(str + len, str_len - len, "%03d", (uint8_t)addr[i]);
    }
}

void HyUtilsStr2Int2Hex(char *str, size_t str_len, char *addr, size_t addr_len)
{
    size_t addr_cnt = 0;
    for (size_t i = 0; i < str_len; i += 3) {
        char buf[4] = {0};
        sscanf(str + i, "%3s", buf);
        uint8_t num = atoi(buf);
        addr[addr_cnt++] = num;
        if (addr_cnt == addr_len) {
            break;
        }
    }
}

uint32_t HyUtilsBitStr2Dec(char *bit_str, size_t len)
{
    uint32_t num = 0;

    for (size_t i = 0; i < len; i++) {
        char buf[2] = {0};
        snprintf(buf, 2, "%c", bit_str[i]);
        // LOGD("buf: %s, %d, %02x \r\n", buf, atoi(buf), atoi(buf) << (len - 1 - i));
        num |= atoi(buf) << (len - 1 - i);
    }

    // LOGD("num: %02x \r\n", num);
    return num;
}

void HyUtilsDec2BitStr(uint32_t num, size_t num_len, char *bit_str, size_t str_len)
{
    size_t len = 0;
    memset(bit_str, '\0', str_len);

    for (size_t i = num_len; i > 0; i--) {
        if (num & (0x1 << (i - 1))) {
            len += snprintf(bit_str + len, str_len - len, "1");
        } else {
            len += snprintf(bit_str + len, str_len - len, "0");
        }
    }
}

