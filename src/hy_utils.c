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
#include <errno.h>

#include "hy_assert.h"

#include "hy_utils.h"

void HyUtilsCheck(void) {}

void HyUtilsIpStr2Int(const char *ip_str, hy_u32_t *ip_num)
{
    HY_ASSERT_RET(!ip_str);

    int num[HY_UTILS_IP_INT_LEN_MAX];

	sscanf(ip_str, "%d.%d.%d.%d", &num[0], &num[1], &num[2], &num[3]);

    *ip_num  = 0;
    for (int i = 0; i < HY_UTILS_IP_INT_LEN_MAX; i++) {
        *ip_num += (num[i] & 0xff) << (8 * (3 - i));
    }
}

void HyUtilsIpInt2Str(hy_u32_t ip_num, char *ip_str, hy_u32_t ip_str_len)
{
    HY_ASSERT_RET(!ip_str);

    hy_u8_t *tmp = (hy_u8_t *)&ip_num;
    hy_u32_t len = 0;

    memset(ip_str, '\0', ip_str_len);

    for (int i = 0; i < HY_UTILS_IP_INT_LEN_MAX; i++) {
        len += snprintf(ip_str + len, ip_str_len - len,
                "%d.", *(tmp + HY_UTILS_IP_INT_LEN_MAX - 1 - i));
    }
    ip_str[len - 1] = '\0';
}

void HyUtilsHex2Int2Str(char *addr, hy_u32_t addr_len, char *str, hy_u32_t str_len)
{
    hy_u32_t len = 0;
    for (hy_u32_t i = 0; i < addr_len; i++) {
        // LOGD("addr: %02x, %d \n", addr[i], (hy_u8_t)addr[i]);
        len += snprintf(str + len, str_len - len, "%03d", (hy_u8_t)addr[i]);
    }
}

void HyUtilsStr2Int2Hex(char *str, hy_u32_t str_len, char *addr, hy_u32_t addr_len)
{
    hy_u32_t addr_cnt = 0;
    for (hy_u32_t i = 0; i < str_len; i += 3) {
        char buf[4] = {0};
        sscanf(str + i, "%3s", buf);
        hy_u8_t num = atoi(buf);
        addr[addr_cnt++] = num;
        if (addr_cnt == addr_len) {
            break;
        }
    }
}

hy_u32_t HyUtilsBitStr2Dec(char *bit_str, hy_u32_t len)
{
    hy_u32_t num = 0;

    for (hy_u32_t i = 0; i < len; i++) {
        char buf[2] = {0};
        snprintf(buf, 2, "%c", bit_str[i]);
        // LOGD("buf: %s, %d, %02x \r\n", buf, atoi(buf), atoi(buf) << (len - 1 - i));
        num |= atoi(buf) << (len - 1 - i);
    }

    // LOGD("num: %02x \r\n", num);
    return num;
}

void HyUtilsDec2BitStr(hy_u32_t num, hy_u32_t num_len, char *bit_str, hy_u32_t str_len)
{
    hy_u32_t len = 0;
    memset(bit_str, '\0', str_len);

    for (hy_u32_t i = num_len; i > 0; i--) {
        if (num & (0x1 << (i - 1))) {
            len += snprintf(bit_str + len, str_len - len, "1");
        } else {
            len += snprintf(bit_str + len, str_len - len, "0");
        }
    }
}

hy_s32_t HyUtilsCheckEndianness(void)
{
    union Check{ //起始地址是一致的
        char a;
        hy_u32_t data;
    } c;

    c.data = 1;

    return (1 == c.a) ? 0 : 1;
}
