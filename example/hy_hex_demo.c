/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_hex_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    12/04 2022 10:09
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        12/04 2022      create the file
 * 
 *     last modified: 12/04 2022 10:09
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_type.h"

#include "hy_hex.h"

int main(int argc, char *argv[])
{
    HyLogInit_m(10 * 1024, HY_LOG_LEVEL_TRACE, HY_LOG_OUTFORMAT_ALL);

    char *buf = "1234567890abcdefghi";
    HY_HEX_ASCII(buf, HY_STRLEN(buf));
    HY_HEX(buf, HY_STRLEN(buf));

    sleep(3);

    HyLogDeInit();

    return 0;
}

