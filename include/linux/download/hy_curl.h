/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_curl.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    29/03 2022 08:17
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        29/03 2022      create the file
 * 
 *     last modified: 29/03 2022 08:17
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_H_
#define __LIBHY_UTILS_INCLUDE_HY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_os_type/hy_os_type.h>

typedef void (*HyCurlGetDataCb_t)(void *buf, hy_u32_t len, void *args);

typedef struct {
    const char          *url;
    const char          *ca_path;
    HyCurlGetDataCb_t   curl_get_data_cb;
    void                *args;
} HyCurlGetData_s;

typedef struct {
} HyCurlConfig_s;

void *HyCurlCreate(HyCurlConfig_s *curl_c);
void HyCurlDestroy(void **handle);

hy_s32_t HyCurlGetDataBlock(void *handle, HyCurlGetData_s *curl_get_data);

hy_s32_t HyCurlGetDataBlock_m(void *handle, const char *url, const char *ca_path,
        HyCurlGetDataCb_t curl_get_data_cb, void *args);

#ifdef __cplusplus
}
#endif

#endif

