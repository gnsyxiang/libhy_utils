/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_log.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/03 2023 15:09
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/03 2023      create the file
 * 
 *     last modified: 17/03 2023 15:09
 */
#ifndef __LIBHY_UTILS_INCLLUDE_HY_LOG_H_
#define __LIBHY_UTILS_INCLLUDE_HY_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

void HyLogCreate(void);

#define LOGF        printf
#define LOGES       printf
#define LOGE        printf
#define LOGW        printf
#define LOGI        printf
#define LOGD        printf
#define LOGT        printf

#ifdef __cplusplus
}
#endif

#endif

