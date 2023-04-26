/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_curl.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    29/03 2022 08:30
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        29/03 2022      create the file
 * 
 *     last modified: 29/03 2022 08:30
 */
#include <stdio.h>

#include <hy_log/hy_log.h>
#include <curl/curl.h>

#include "hy_assert.h"
#include "hy_barrier.h"
#include "hy_mem.h"

#include "hy_curl.h"

typedef struct {
    CURL *curl;
} _curl_context_s;

static size_t _curl_write_cb(void *buffer, size_t size, size_t nmemb,
        void *args)
{
    HY_ASSERT(buffer);
    HY_ASSERT(args);

    HyCurlGetData_s *curl_get_data = args;

    if (curl_get_data->curl_get_data_cb) {
        curl_get_data->curl_get_data_cb(buffer,
                size * nmemb, curl_get_data->args);
    }

    LOGD("ota get file size: %d \n", (hy_u32_t)(size * nmemb));

    return nmemb;
}

hy_s32_t HyCurlGetDataBlock(void *handle, HyCurlGetData_s *curl_get_data)
{
    LOGT("handle: %p, cur_get_data: %p \n", handle, curl_get_data);
    HY_ASSERT_RET_VAL(!handle || !curl_get_data, -1);

    _curl_context_s *context = handle;
    CURLcode res;

    curl_easy_setopt(context->curl, CURLOPT_URL, curl_get_data->url);
    curl_easy_setopt(context->curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(context->curl, CURLOPT_WRITEFUNCTION, _curl_write_cb);
    curl_easy_setopt(context->curl, CURLOPT_WRITEDATA, curl_get_data);
    if (NULL == curl_get_data->ca_path) {
        curl_easy_setopt(context->curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(context->curl, CURLOPT_SSL_VERIFYHOST, 0);
    } else {
        curl_easy_setopt(context->curl, CURLOPT_SSL_VERIFYPEER, 1);
        curl_easy_setopt(context->curl, CURLOPT_CAINFO, curl_get_data->ca_path);
    }

    res = curl_easy_perform(context->curl);
    if (res != CURLE_OK) {
        LOGE("curl_easy_perform failed \n");
        return -1;
    } else {
        return 0;
    }
}

void HyCurlDestroy(void **handle)
{
    LOGT("&handle: %p, handle: %p \n", handle, *handle);
    HY_ASSERT_RET(!handle || !*handle);

    _curl_context_s *context = *handle;

    curl_easy_cleanup(context->curl);

    HY_MEM_FREE_PP(handle);
}

void *HyCurlCreate(HyCurlConfig_s *curl_c)
{
    LOGT("curl_c: %p \n", curl_c);
    HY_ASSERT_RET_VAL(!curl_c, NULL);

    _curl_context_s *context = NULL;

    do {
        context = HY_MEM_MALLOC_BREAK(_curl_context_s *, sizeof(*context));

        context->curl = curl_easy_init();
        if (!context->curl) {
            LOGE("curl_easy_init failed \n");
            break;
        }

        LOGI("curl create successful, context: %p \n", context);
        return context;
    } while (0);

    LOGE("curl create failed \n");
    HyCurlDestroy((void **)&context);
    return NULL;
}

