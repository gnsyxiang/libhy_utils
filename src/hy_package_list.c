/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_package_list.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/04 2023 11:32
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/04 2023      create the file
 * 
 *     last modified: 21/04 2023 11:32
 */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <hy_log/hy_log.h>

#include "hy_package_list.h"
#include "hy_mem.h"

struct HyPackageList_s {
    HyPackageListSaveConfig_s   save_c;

    pthread_mutex_t             mutex;
    struct hy_list_head         list;
};

hy_u32_t HyPackageListGetNodeCount(HyPackageList_s *context)
{
    if (!context) {
        LOGE("the param is NULL \n");
    }

    hy_u32_t cnt;
    pthread_mutex_lock(&context->mutex);
    cnt = context->save_c.num;
    pthread_mutex_unlock(&context->mutex);

    return cnt;
}

HyPackageListNode_s *HyPackageListHeadGet(HyPackageList_s *context)
{
    HyPackageListNode_s *pos;

    pthread_mutex_lock(&context->mutex);
    pos = hy_list_first_entry(&context->list, HyPackageListNode_s, entry);
    hy_list_del(&pos->entry);
    context->save_c.num--;
    pthread_mutex_unlock(&context->mutex);

    return pos;
}

void HyPackageListTailPut(HyPackageList_s *context, HyPackageListNode_s *node)
{
    if (!context || !node) {
        LOGE("the param is NULL \n");
    }

    pthread_mutex_lock(&context->mutex);
    context->save_c.num++;
    hy_list_add_tail(&node->entry, &context->list);
    pthread_mutex_unlock(&context->mutex);
}

void HyPackageListDestroy(HyPackageList_s **context_pp)
{
    if (!context_pp || !*context_pp) {
        LOGE("the param is NULL \n");
    }

    HyPackageListNode_s *pos, *n;
    HyPackageList_s *context = *context_pp;
    HyPackageListSaveConfig_s *save_c = &context->save_c;

    if (context) {
        if (&context->mutex) {
            pthread_mutex_lock(&context->mutex);
            hy_list_for_each_entry_safe(pos, n, &context->list, entry) {
                hy_list_del(&pos->entry);
                pthread_mutex_unlock(&context->mutex);

                if (save_c->node_destroy_cb) {
                    save_c->node_destroy_cb(pos);
                }

                pthread_mutex_lock(&context->mutex);
            }
            pthread_mutex_unlock(&context->mutex);
        }

        pthread_mutex_destroy(&context->mutex);
    }

    LOGI("package list destroy, context: %p \n", context);
    free(context);
    *context_pp = NULL;
}

HyPackageList_s *HyPackageListCreate(HyPackageListConfig_s *config)
{
    HyPackageList_s *context = NULL;
    do {
        context = HY_MEM_CALLOC_BREAK(HyPackageList_s *, sizeof(*context));

        memcpy(&context->save_c, &config->save_c, sizeof(context->save_c));

        HY_INIT_LIST_HEAD(&context->list);

        if (0 != pthread_mutex_init(&context->mutex, NULL)) {
            LOGES("pthread_mutex_init failed \n");
            break;
        }

        HyPackageListNode_s *node;
        HyPackageListSaveConfig_s *save_c = &context->save_c;
        hy_u32_t num = context->save_c.num;

        if (!save_c->node_create_cb) {
            LOGE("the node_create_cb is NULL \n");
            break;
        }

        while (num-- != 0) {
            node = save_c->node_create_cb();
            if (!node) {
                LOGE("node_create_cb failed \n");
                continue;
            }

            pthread_mutex_lock(&context->mutex);
            hy_list_add_tail(&node->entry, &context->list);
            pthread_mutex_unlock(&context->mutex);
        }

        LOGI("package list create, context: %p \n", context);
        return context;
    }while (0);

    LOGI("package list failed \n");
    HyPackageListDestroy(&context);
    return NULL;
}
