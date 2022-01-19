/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_net.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    16/12 2021 18:57
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        16/12 2021      create the file
 * 
 *     last modified: 16/12 2021 18:57
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"

#include "hy_net.h"

static hy_s32_t _get_cmd_ret(const char *cmd_buf)
{
    FILE *stream;
    char recv_buf[32] = {0};

    stream = popen(cmd_buf, "r");
    if (!stream) {
        LOGE("failed, error: %s \n", strerror(errno));
        return -1;
    }

    fread(recv_buf, sizeof(char), sizeof(recv_buf) - 1, stream);
    pclose(stream);

    if (atoi(recv_buf) > 0) {
        return 0;
    }

    return -1;
}

hy_s32_t HyNetIsConnect(const char *dst)
{
    HY_ASSERT_RET_VAL(!dst, 0);

    char cmd_buf[256] = {0};

    snprintf(cmd_buf, 256, "ping %s -c1 -w2 -4 | grep time= | wc -l", dst);
    if (0 == _get_cmd_ret(cmd_buf)) {
        return 1;
    }

    memset(cmd_buf, '\0', sizeof(cmd_buf));
    snprintf(cmd_buf, 256, "ping %s -c1 -w2 -6 | grep time= | wc -l", dst);
    if (0 == _get_cmd_ret(cmd_buf)) {
        return 1;
    }

    return 0;
}
