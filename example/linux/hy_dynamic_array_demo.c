/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_dynamic_array_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/03 2023 16:43
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/03 2023      create the file
 * 
 *     last modified: 17/03 2023 16:43
 */
#include <stdio.h>
#include <string.h>

#include <hy_log/hy_log.h>

#include <hy_os/hy_mem.h>

#include "hy_dynamic_array.h"

int main(int argc, char *argv[])
{
    char buf[128] = {0};
    char *data = NULL;

    HyDynamicArray_s *handle = HyDynamicArrayCreate(4, 12);

    data = "1234";
    HyDynamicArrayWrite(handle, data, strlen(data));

    HY_MEMSET(buf, sizeof(buf));
    HyDynamicArrayRead(handle, buf, 4);
    LOGI("-1-buf: %s \n", buf);

    data = "5678";
    HyDynamicArrayWrite(handle, data, strlen(data));

    HY_MEMSET(buf, sizeof(buf));
    HyDynamicArrayRead(handle, buf, 8);
    LOGI("-2-buf: %s \n", buf);

    HyDynamicArrayDestroy(&handle);

    return 0;
}

