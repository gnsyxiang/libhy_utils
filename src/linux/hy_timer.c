/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_timer.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 13:37
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 13:37
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "hy_timer.h"

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_type.h"
#include "hy_hal/hy_thread.h"
#include "hy_hal/hy_string.h"

#include "hy_list.h"
#include "hy_log.h"

/**
 * @brief 链表实现的定时器
 */

typedef struct {
    HyTimerConfig_t     timer_config;

    size_t              rotation;           ///< 旋转的圈数，类比分针走一圈

    struct hy_list_head list;
} _timer_t;

typedef struct {
    HyTimerServiceSaveConfig_t  save_config;

    uint32_t                    cur_slot;
    pthread_mutex_t             mutex;

    pthread_t                   id;
    int                         exit_flag;
    void                        *thread_handle;

    struct hy_list_head         *list_head;
} _timer_context_t;

static _timer_context_t *context = NULL;

void *HyTimerAdd(HyTimerConfig_t *timer_config)
{
    LOGT("\n");
    HY_ASSERT_VAL_RET_VAL(!timer_config, NULL);

    _timer_t *timer = HY_MEM_MALLOC_RET_VAL(_timer_t *, sizeof(*timer), NULL);

    HY_MEMCPY(&timer->timer_config, timer_config, sizeof(*timer_config));

    hy_u32_t slot_num = context->save_config.slot_num;
    timer->rotation = timer_config->expires / slot_num;     // 圈数
    size_t slot     = timer_config->expires % slot_num;     // 落在哪个格子

    LOGD("rotation: %d, slot: %d \n", timer->rotation, slot);

    pthread_mutex_lock(&context->mutex);
    hy_list_add_tail(&timer->list, &context->list_head[slot]);
    pthread_mutex_unlock(&context->mutex);

    return timer;
}

void HyTimerDel(void **timer_handle)
{
    HY_ASSERT_VAL_RET(!timer_handle || !*timer_handle);

    _timer_t *pos, *n;
    uint32_t i;

    for (i = 0; i < context->save_config.slot_num; ++i) {
        pthread_mutex_lock(&context->mutex);

        hy_list_for_each_entry_safe(pos, n, &context->list_head[i], list) {
            if (*timer_handle == pos) {
                hy_list_del(&pos->list);

                HY_MEM_FREE_PP(&pos);
                *timer_handle = NULL;

                pthread_mutex_unlock(&context->mutex);
                goto DEL_ERR_1;
            }
        }

        pthread_mutex_unlock(&context->mutex);
    }

DEL_ERR_1:
    return;
}

static hy_s32_t _timer_loop_cb(void *args)
{
    struct timeval tv;
    int err;
    _timer_t *pos, *n;
    time_t sec, usec;
    HyTimerConfig_t *timer_config = NULL;
    hy_u32_t slot_num = context->save_config.slot_num;

    sec = context->save_config.slot_interval_ms / 1000;
    usec = (context->save_config.slot_interval_ms % 1000) * 1000;

    while (!context->exit_flag) {
        tv.tv_sec   = sec;
        tv.tv_usec  = usec;

        do {
            err = select(0, NULL, NULL, NULL, &tv);
        } while(err < 0 && errno == EINTR);

        pthread_mutex_lock(&context->mutex);
        hy_list_for_each_entry_safe(pos, n, &context->list_head[context->cur_slot], list) {
            if (pos->rotation > 0) {
                pos->rotation--;
            } else {
                timer_config = &pos->timer_config;

                if (timer_config->timer_cb) {
                    timer_config->timer_cb(timer_config->args);
                }

                hy_list_del(&pos->list);

                if (pos->timer_config.repeat_flag == HY_TIMER_MODE_REPEAT) {
                    pos->rotation = timer_config->expires / slot_num;
                    size_t slot     = timer_config->expires % slot_num;
                    slot += context->cur_slot;
                    slot %= slot_num;

                    hy_list_add_tail(&pos->list, &context->list_head[slot]);
                } else {
                    HY_MEM_FREE_PP(&pos);
                }
            }
        }
        pthread_mutex_unlock(&context->mutex);

        context->cur_slot++;
        context->cur_slot %= slot_num;
    }

    return -1;
}

void HyTimerDestroy(void **handle)
{
    HY_ASSERT_VAL_RET(!handle || !*handle);

    context->exit_flag = 1;
    HyThreadDestroy(&context->thread_handle);

    _timer_t *pos, *n;
    for (uint32_t i = 0; i < context->save_config.slot_num; ++i) {
        pthread_mutex_lock(&context->mutex);

        hy_list_for_each_entry_safe(pos, n, &context->list_head[i], list) {
            hy_list_del(&pos->list);

            HY_MEM_FREE_PP(&pos);
        }

        pthread_mutex_unlock(&context->mutex);
    }

    pthread_mutex_destroy(&context->mutex);

    HY_MEM_FREE_PP(&context->list_head);

    LOGI("timer destroy, handle: %p \n", context);
    HY_MEM_FREE_PP(&context);
}

void *HyTimerCreate(HyTimerServiceConfig_t *config)
{
    HY_ASSERT_VAL_RET_VAL(!config, NULL);

    do {
        context = HY_MEM_MALLOC_BREAK(_timer_context_t *, sizeof(*context));
        HY_MEMCPY(&context->save_config, &config->save_config, sizeof(config->save_config));

        context->list_head = HY_MEM_MALLOC_BREAK(struct hy_list_head *,
                sizeof(struct hy_list_head) * config->save_config.slot_num);

        for (uint32_t i = 0; i < config->save_config.slot_num; ++i) {
            HY_INIT_LIST_HEAD(&context->list_head[i]);
        }

        pthread_mutex_init(&context->mutex, NULL);

        context->thread_handle = HyThreadCreate_m("hy_timer", _timer_loop_cb, context);
        if (!context->thread_handle) {
            LOGE("failed \n");
            break;
        }

        LOGI("timer create, handle: %p \n", context);
        return context;
    } while (0);

    HyTimerDestroy(NULL);
    return NULL;
}
