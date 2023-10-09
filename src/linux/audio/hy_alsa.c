/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_alsa.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/07 2021 16:37
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/07 2021      create the file
 * 
 *     last modified: 30/07 2021 16:37
 */
#include <stdio.h>

#include "hy_mem.h"
#include "hy_assert.h"
#include "hy_string.h"
#include "hy_log.h"

#include "hy_alsa.h"

#define ALONE_DEBUG 1

typedef struct {
    HyAlsaSaveConfig_t save_config;

    snd_pcm_t   *alsa_handle;

    /**
     * @brief The Idle frame number that can be read.
     */
    snd_pcm_uframes_t frame_num;

    /**
     * @brief The number of bit per sample point.
     */
    size_t bits_per_sample;

    /**
     * @brief The number of bit per frame.
     */
    size_t bits_per_frame;
} _alsa_context_t;

void HyAlsaPutData(void *handle, void *data, size_t len)
{
}

static ssize_t read_pcm(_alsa_context_t *context, void *buf)
{
    ssize_t r;
    size_t result = 0;
    size_t count = context->frame_num;

    while (count > 0) {
        //录count个frame到data中
        r = snd_pcm_readi(context->alsa_handle, buf, count);

        if (r == -EAGAIN || (r >= 0 && (size_t)r < count)) {
            snd_pcm_wait(context->alsa_handle, 1000);
        } else if (r == -EPIPE) {
            snd_pcm_prepare(context->alsa_handle);
            LOGE("<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>> \n");
        } else if (r == -ESTRPIPE) {
            LOGE("<<<<<<<<<<<<<<< Need suspend >>>>>>>>>>>>>>> \n");
        } else if (r < 0) {
            LOGE("snd_pcm_readi: [%s] \n", snd_strerror(r));
            return -1;
        }

        if (r > 0) {
            result += r;
            count -= r;
        }
    }
    return result;
}

hy_s32_t HyAlsaGetData(void *handle, void *data)
{
    HY_ASSERT_NULL_RET_VAL(!handle || !data, -1);

    return read_pcm((_alsa_context_t *)handle, data);
}

void HyAlsaDestroy(void **handle)
{
    LOGT("%s:%d \n", __func__, __LINE__);
    HY_ASSERT_NULL_RET(!handle || !*handle);

    _alsa_context_t *context = *handle;

	snd_pcm_drain(context->alsa_handle);
	snd_pcm_close(context->alsa_handle);

    HY_FREE(handle);
}

void *HyAlsaCreate(HyAlsaConfig_t *alsa_config)
{
    LOGT("%s:%d \n", __func__, __LINE__);
    _alsa_context_t *context = NULL;
    snd_pcm_hw_params_t *hwparams = NULL;

    do {
        context = (_alsa_context_t *)HY_MALLOC_BREAK(sizeof(*context));

        HY_MEMCPY(&context->save_config, &alsa_config->save_config);

        if (snd_pcm_open(&context->alsa_handle,
                    alsa_config->save_config.snd_dev_name,
                    SND_PCM_STREAM_CAPTURE, 0) < 0) {
            LOGE("snd_pcm_open [ %s] \n", alsa_config->save_config.snd_dev_name);
            break;
        }

        if (snd_pcm_hw_params_malloc(&hwparams) < 0) {
            LOGE("snd_pcm_hw_params_malloc faild  \n");
            break;
        }

        // 获取声卡的默认配置
        if (snd_pcm_hw_params_any(context->alsa_handle, hwparams) < 0) {
            LOGE("snd_pcm_hw_params_any \n");
            break;
        }

        /*
         * 设置交错访问方式
         * snd_pcm_readi/snd_pcm_writei access
         * SND_PCM_ACCESS_RW_INTERLEAVED
         */ 
        if (snd_pcm_hw_params_set_access(context->alsa_handle,
                    hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
            LOGE("snd_pcm_hw_params_set_access \n");
            break;
        }

        // 设置数据格式
        if (snd_pcm_hw_params_set_format(context->alsa_handle,
                    hwparams, snd_pcm_format_value(alsa_config->save_config.format_name)) < 0) {
            LOGE("snd_pcm_hw_params_set_format \n");
            break;
        }

        // 设置音频通道
        if (snd_pcm_hw_params_set_channels(context->alsa_handle,
                    hwparams, alsa_config->save_config.channels) < 0) {
            LOGE("snd_pcm_hw_params_set_channels \n");
            break;
        }

        hy_u32_t exact_rate;
        /* Set sample rate. If the exact rate is not supported */
        /* by the hardware, use nearest possible rate.		 */
        exact_rate = alsa_config->save_config.sample_rate;

        // 设置音频数据的最接近目标的采样率
        if (snd_pcm_hw_params_set_rate_near(context->alsa_handle,
                    hwparams, &exact_rate, 0) < 0) {
            LOGE("snd_pcm_hw_params_set_rate_near \n");
            break;
        }

        if (alsa_config->save_config.sample_rate != exact_rate) {
            LOGE("The rate %d Hz is not supported by your hardware. ==> Using %d Hz instead. \n",
                    alsa_config->save_config.sample_rate, exact_rate);
        }

        context->save_config.sample_rate = exact_rate;

        hy_u32_t buffer_time;
        if (snd_pcm_hw_params_get_buffer_time_max(hwparams,
                    &buffer_time, 0) < 0) {
            LOGE("snd_pcm_hw_params_get_buffer_time_max \n");
            break;
        }

        LOGI("buffer_time: %d \n", buffer_time);
        if (!alsa_config->save_config.period_time) {
            if (buffer_time > 500000) 
                buffer_time = 500000;
            context->save_config.period_time = buffer_time / 4;
        } else {
            buffer_time = alsa_config->save_config.period_time * 5;
        }

        if (snd_pcm_hw_params_set_buffer_time_near(context->alsa_handle,
                    hwparams, &buffer_time, 0) < 0) {
            LOGE("snd_pcm_hw_params_set_buffer_time_near \n");
            break;
        }

        if (snd_pcm_hw_params_set_period_time_near(context->alsa_handle,
                    hwparams, &context->save_config.period_time, 0) < 0) {
            LOGE("snd_pcm_hw_params_set_period_time_near \n");
            break;
        }

        /* Set hw params */
        if (snd_pcm_hw_params(context->alsa_handle, hwparams) < 0) {
            LOGE("snd_pcm_hw_params(handle, params) \n");
            break;
        }

        snd_pcm_uframes_t alsa_buffer_size;
        snd_pcm_hw_params_get_period_size(hwparams, &context->frame_num, 0);
        snd_pcm_hw_params_get_buffer_size(hwparams, &alsa_buffer_size);
        if (context->frame_num == alsa_buffer_size) {
            LOGE(("Can't use period equal to buffer size (%lu == %lu) \n"),
                    context->frame_num, alsa_buffer_size);
            break;
        }

        //通过fomart获取每sample的bit数
        context->bits_per_sample = snd_pcm_format_physical_width(snd_pcm_format_value(alsa_config->save_config.format_name));

        //把frame定义为一个sample的数据，包括每个channel
        context->bits_per_frame = context->bits_per_sample * alsa_config->save_config.channels;

        snd_pcm_hw_params_free(hwparams);

        return context;
    } while (0);

    if (hwparams) {
        snd_pcm_hw_params_free(hwparams);
    }

    HyAlsaDestroy((void **)&context);
    return NULL;
}

