/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_utils_linux.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/03 2022 19:14
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/03 2022      create the file
 * 
 *     last modified: 30/03 2022 19:14
 */
#include <stdio.h>
#include <sys/time.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_barrier.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_log.h"

#include "hy_utils_linux.h"

hy_u32_t HyUtilsLinuxRandomNum(hy_u32_t range)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);

    return (1 + (hy_u32_t)(1.0 * range * rand() / (RAND_MAX + 1.0)));
}

