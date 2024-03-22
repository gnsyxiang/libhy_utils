/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_math.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    09/10 2023 15:46
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        09/10 2023      create the file
 * 
 *     last modified: 09/10 2023 15:46
 */
#include <stdio.h>
#include<stdlib.h>

#include <hy_os/hy_time.h>

#include "hy_math.h"

hy_u32_t HyMathGenerateRandom(hy_u32_t range)
{
    srand(HyTimeGetUTC_us());

    return (rand() % range);
}
