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

#include "hy_list.h"

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_log.h"

#define ALONE_DEBUG 1

typedef struct {
    HyTimerConfig_t timer_config;

    size_t rotation;

    struct hy_list_head list;
} _timer_t;

typedef struct {
    HyTimerServiceSaveConfig_t save_config;

    uint32_t cur_slot;
    pthread_mutex_t mutex;

    pthread_t id;
    int exit_flag;

    struct hy_list_head *list_head;
} _timer_context_t;

static _timer_context_t *context = NULL;

void *HyTimerAdd(HyTimerConfig_t *timer_config)
{
    HY_ASSERT_VAL_RET_VAL(!timer_config, NULL);

    _timer_t *timer = HY_MEM_MALLOC_RET_VAL(_timer_t *, sizeof(*timer), NULL);

    HY_MEMCPY(&timer->timer_config, timer_config, sizeof(*timer_config));

    timer->rotation = timer_config->expires / context->save_config.slot_num;
    size_t slot     = timer_config->expires % context->save_config.slot_num;

    hy_list_add_tail(&timer->list, &context->list_head[slot]);

    return timer;
}

void HyTimerDel(void **timer_handle)
{
    HY_ASSERT_VAL_RET(!timer_handle || !*timer_handle);

    _timer_t *pos, *n;
    uint32_t i;

    for (i = 0; i < context->save_config.slot_num; ++i) {
        hy_list_for_each_entry_safe(pos, n, &context->list_head[i], list) {
            if (*timer_handle == pos) {
                hy_list_del(&pos->list);

                pthread_mutex_lock(&context->mutex);
                HY_MEM_FREE_PP(&pos);
                *timer_handle = NULL;
                pthread_mutex_unlock(&context->mutex);

                goto DEL_ERR_1;
            }
        }
    }

DEL_ERR_1:
    return;
}

static void *_timer_loop_cb(void *args)
{
    struct timeval tv;
    int err;
    _timer_t *pos, *n;
    time_t sec, usec;
    HyTimerConfig_t *timer_config = NULL;

    sec = context->save_config.slot_interval_ms / 1000;
    usec = (context->save_config.slot_interval_ms % 1000) * 1000;

    while (!context->exit_flag) {
        tv.tv_sec   = sec;
        tv.tv_usec  = usec;

        do {
            err = select(0, NULL, NULL, NULL, &tv);
        } while(err < 0 && errno == EINTR);

        hy_list_for_each_entry_safe(pos, n, &context->list_head[context->cur_slot], list) {
            if (pos->rotation > 0) {
                pos->rotation--;
            } else {
                timer_config = &pos->timer_config;

                pthread_mutex_lock(&context->mutex);
                if (timer_config->timer_cb) {
                    timer_config->timer_cb(timer_config->args);
                }
                pthread_mutex_unlock(&context->mutex);

                hy_list_del(&pos->list);

                if (pos->timer_config.repeat_flag == HY_TIMER_MODE_REPEAT) {
                    pos->rotation = timer_config->expires / context->save_config.slot_num;
                    size_t slot     = timer_config->expires % context->save_config.slot_num;
                    slot += context->cur_slot;
                    slot %= context->save_config.slot_num;

                    hy_list_add_tail(&pos->list, &context->list_head[slot]);
                } else {
                    HY_MEM_FREE_PP(&pos);
                }
            }
        }

        context->cur_slot++;
        context->cur_slot %= context->save_config.slot_num;
    }

    return NULL;
}

void HyTimerDestroy(void **handle)
{
    context->exit_flag = 1;
    pthread_join(context->id, NULL);

    _timer_t *pos, *n;
    for (uint32_t i = 0; i < context->save_config.slot_num; ++i) {
        hy_list_for_each_entry_safe(pos, n, &context->list_head[i], list) {
            hy_list_del(&pos->list);

            HY_MEM_FREE_PP(&pos);
        }
    }

    pthread_mutex_destroy(&context->mutex);

    HY_MEM_FREE_PP(&context->list_head);

    HY_MEM_FREE_PP(&context);

    LOGI("timer destroy successful \n");
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

        pthread_create(&context->id, NULL, _timer_loop_cb, context);

        LOGI("timer create successful \n");
        return context;
    } while (0);

    HyTimerDestroy(NULL);
    return NULL;
}
