/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_md5sum_test.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 13:56
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 13:56
 */
#include <stdio.h>
#include <string.h>

#include "hy_md5sum.h"

int main(int argc, const char *argv[])
{
    uint8_t digest[16] = {0};
    uint8_t md5sum[16] = {
        0xe8, 0x07, 0xf1, 0xfc, 0xf8, 0x2d, 0x13, 0x2f,
        0x9b, 0xb0, 0x18, 0xca, 0x67, 0x38, 0xa1, 0x9f
    };
    uint8_t msg[11] = {"1234567890"};

    printf("Encrypted data: %s \n", msg);

    HyMd5sum(msg, strlen((char *)msg), digest);

    for (int32_t i = 0; i < 16; i++) {
        printf("%2.2x", digest[i]);
    }
    printf("\n");

    printf("result: %s \n", \
            memcmp(digest, md5sum, 16) == 0 ? "successful" : "failed");

    return 0;
}
