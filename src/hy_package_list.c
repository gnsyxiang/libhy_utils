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

#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_thread_mutex.h"
#include "hy_thread_cond.h"
#include "hy_package_list.h"

struct HyPackageList_s {
    HyPackageListSaveConfig_s   save_c;
    hy_s32_t                    is_exit;

    HyThreadCond_s              *cond_h;
    HyThreadMutex_s             *mutex_h;
    struct hy_list_head         list;
};

hy_u32_t HyPackageListGetNodeCount(HyPackageList_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);
    hy_u32_t cnt;

    HyThreadMutexLock_m(handle->mutex_h);
    cnt = handle->save_c.num;
    HyThreadMutexUnLock_m(handle->mutex_h);

    return cnt;
}

HyPackageListNode_s *HyPackageListHeadGet(HyPackageList_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, NULL);
    HyPackageListNode_s *pos;

    HyThreadMutexLock_m(handle->mutex_h);
    while (handle->save_c.num == 0) {
        HyThreadCondWait_m(handle->cond_h, handle->mutex_h, 0);

        if (1 == handle->is_exit) {
            HyThreadMutexUnLock_m(handle->mutex_h);
            return NULL;
        }
    }
    pos = hy_list_first_entry(&handle->list, HyPackageListNode_s, entry);
    hy_list_del(&pos->entry);
    handle->save_c.num--;
    HyThreadMutexUnLock_m(handle->mutex_h);

    return pos;
}

void HyPackageListTailPut(HyPackageList_s *handle, HyPackageListNode_s *node)
{
    HY_ASSERT_RET(!handle || !node);

    HyThreadMutexLock_m(handle->mutex_h);
    handle->save_c.num++;
    hy_list_add_tail(&node->entry, &handle->list);
    HyThreadMutexUnLock_m(handle->mutex_h);

    HyThreadCondSignal_m(handle->cond_h);
}

void HyPackageListDestroy(HyPackageList_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyPackageListNode_s *pos, *n;
    HyPackageList_s *handle = *handle_pp;
    HyPackageListSaveConfig_s *save_c = &handle->save_c;

    if (handle) {
        handle->is_exit = 1;

        HyThreadCondBroadcast_m(handle->cond_h);

        if (handle->mutex_h) {
            HyThreadMutexLock_m(handle->mutex_h);
            hy_list_for_each_entry_safe(pos, n, &handle->list, entry) {
                hy_list_del(&pos->entry);
                HyThreadMutexUnLock_m(handle->mutex_h);

                if (save_c->node_destroy_cb) {
                    save_c->node_destroy_cb(&pos);
                }

                HyThreadMutexLock_m(handle->mutex_h);
            }
            HyThreadMutexUnLock_m(handle->mutex_h);

            HyThreadMutexDestroy(&handle->mutex_h);
        }

        if (handle->cond_h) {
            HyThreadCondDestroy(&handle->cond_h);
        }
    }

    LOGI("package list destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyPackageList_s *HyPackageListCreate(HyPackageListConfig_s *package_list_c)
{
    HY_ASSERT_RET_VAL(!package_list_c, NULL);
    HyPackageList_s *handle = NULL;

    do {
        handle = HY_MEM_CALLOC_BREAK(HyPackageList_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &package_list_c->save_c, sizeof(handle->save_c));

        HY_INIT_LIST_HEAD(&handle->list);

        HyThreadMutexConfig_s mutex_c;
        HY_MEMSET(&mutex_c, sizeof(mutex_c));
        handle->mutex_h = HyThreadMutexCreate(&mutex_c);
        if (!handle->mutex_h) {
            LOGE("HyThreadMutexCreate failed \n");
            break;
        }

        HyThreadCondConfig_s cond_c;
        HY_MEMSET(&cond_c, sizeof(cond_c));
        handle->cond_h = HyThreadCondCreate(&cond_c);
        if (!handle->cond_h) {
            LOGE("HyThreadCondCreate failed \n");
            break;
        }

        HyPackageListSaveConfig_s *save_c = &handle->save_c;
        if (!save_c->node_create_cb) {
            LOGE("the node_create_cb is NULL \n");
            break;
        }

        HyPackageListNode_s *node;
        hy_u32_t num = handle->save_c.num;
        while (num-- != 0) {
            node = save_c->node_create_cb();
            if (!node) {
                LOGE("node_create_cb failed \n");
                continue;
            }

            HyThreadMutexLock_m(handle->mutex_h);
            hy_list_add_tail(&node->entry, &handle->list);
            HyThreadMutexUnLock_m(handle->mutex_h);
        }

        LOGI("package list create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGI("package list create failed \n");
    HyPackageListDestroy(&handle);
    return NULL;
}
