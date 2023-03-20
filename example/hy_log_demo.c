/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    main.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/03 2021 20:22
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/03 2021      create the file
 * 
 *     last modified: 18/03 2021 20:22
 */
#include <stdio.h>

#include "hy_mem.h"
#include "hy_log.h"

int main(int argc, char const* argv[])
{
    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(log_c));
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.level              = HY_LOG_LEVEL_TRACE;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL;

    if (0 != HyLogInit(&log_c)) {
        printf("HyLogInit error \n");
        return -1;
    }

    LOGT("-1-hello log \n");
    LOGD("-2-hello log \n");
    LOGI("-3-hello log \n");
    LOGW("-4-hello log \n");
    LOGE("-5-hello log \n");
    LOGES("-6-hello log \n");

    HyLogDeInit();

    return 0;
}

