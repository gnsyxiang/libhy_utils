/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_utils_test.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 14:16
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 14:16
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#include "hy_utils.h"

int main(int argc, char *argv[])
{
    void *log_h = HyLogCreate_m(512, 512,
            HY_LOG_LEVEL_TRACE, HY_TYPE_FLAG_ENABLE);
    if (!log_h) {
        LOGE("HyLogCreate_m failed \n");
        return -1;
    }

    LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    {
        hy_u32_t ip_num = 0;
        char *ip_str = "192.168.1.110";

        HyUtilsIpStr2Int(ip_str, &ip_num);
        LOGE("ip_num: %u \n", ip_num);
    }

    {
        uint32_t ip_num = 3232235886;
        char ip_str[IP_STR_DOT_LEN] = {0};

        HyUtilsIpInt2Str(ip_num, ip_str, sizeof(ip_str));
        LOGE("ip_str: %s \n", ip_str);
    }

    {
        char addr[] = {0x10, 0x11, 0x12};
        char buf[BUF_LEN] = {0};

        HyUtilsHex2Int2Str(addr, HyHalUtilsArrayCnt(addr), buf, sizeof(buf));
        LOGE("buf: %s \n", buf);
    }

    {
        char *buf = "016017018";
        char addr[3];

        HyUtilsStr2Int2Hex(buf, strlen(buf), addr, 3);
        for (int i = 0; i < 3; ++i) {
            LOGE("%02x \n", addr[i]);
        }
    }

    {
        hy_u32_t dec = 0;
        char *buf = "10101010";
        dec = HyUtilsBitStr2Dec(buf, strlen(buf));
        LOGE("dec: %d, %02x \n", dec, dec);
    }

    {
        uint32_t dec = 0xaa;
        char buf[BUF_LEN] = {0};

        HyUtilsDec2BitStr(dec, 8, buf, sizeof(buf));
        LOGE("buf: %s \n", buf);
    }

    HyLogDestroy(&log_h);

    return 0;
}

