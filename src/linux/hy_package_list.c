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
#include <unistd.h>

#include <hy_os/hy_assert.h>
#include <hy_os/hy_mem.h>
#include <hy_os/hy_thread_mutex.h>
#include <hy_os/hy_thread_cond.h>

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
    hy_u32_t cnt;

    HY_ASSERT_RET_VAL(!handle || handle->is_exit == 1, -1);

    HyThreadMutexLock_m(handle->mutex_h);
    cnt = handle->save_c.num;
    HyThreadMutexUnLock_m(handle->mutex_h);

    return cnt;
}

HyPackageListNode_s *HyPackageListHeadGet(HyPackageList_s *handle)
{
    HyPackageListNode_s *pos;

    HY_ASSERT_RET_VAL(!handle || handle->is_exit == 1, NULL);

    HyThreadMutexLock_m(handle->mutex_h);
    while (handle->save_c.num == 0) {
        HyThreadCondWait_m(handle->cond_h, handle->mutex_h, 0);

        if (1 == handle->is_exit) {
            HyThreadMutexUnLock_m(handle->mutex_h);

            LOGI("called destroy for exit \n");
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
    HY_ASSERT_RET(!handle || !node || handle->is_exit == 1);

    HyThreadMutexLock_m(handle->mutex_h);
    handle->save_c.num++;
    hy_list_add_tail(&node->entry, &handle->list);
    HyThreadMutexUnLock_m(handle->mutex_h);

    HyThreadCondSignal_m(handle->cond_h);
}

void HyPackageListDestroy(HyPackageList_s **handle_pp)
{
    HyPackageListNode_s *pos, *n;
    HyPackageList_s *handle = *handle_pp;
    HyPackageListSaveConfig_s *save_c = &handle->save_c;

    HY_ASSERT_RET(!handle_pp || !*handle_pp);

    // 保证在之后的读取和写入都直接返回
    handle->is_exit = 1;

    // 保证读取要么操作完成，要么在信号量上等待
    usleep(1 * 1000);

    // 唤醒所有等待信号量的操作
    HyThreadCondBroadcast_m(handle->cond_h);

    // 保证信号量被唤醒后，锁正常释放
    usleep(1 * 1000);

    HyThreadMutexLock_m(handle->mutex_h);
    hy_list_for_each_entry_safe(pos, n, &handle->list, entry) {
        hy_list_del(&pos->entry);

        if (save_c->node_destroy_cb) {
            save_c->node_destroy_cb(&pos);
        }
    }
    HyThreadMutexUnLock_m(handle->mutex_h);

    HyThreadMutexDestroy(&handle->mutex_h);

    HyThreadCondDestroy(&handle->cond_h);

    LOGI("package list destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyPackageList_s *HyPackageListCreate(HyPackageListConfig_s *package_list_c)
{
    HyPackageList_s *handle = NULL;
    HyPackageListNode_s *node;
    hy_u32_t num;
    HyPackageListSaveConfig_s *save_c;

    save_c = &package_list_c->save_c;

    HY_ASSERT_RET_VAL(!package_list_c || !save_c->node_create_cb || !save_c->node_destroy_cb, NULL);

    do {
        handle = HY_MEM_CALLOC_BREAK(HyPackageList_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, save_c, sizeof(handle->save_c));

        HY_INIT_LIST_HEAD(&handle->list);

        handle->mutex_h = HyThreadMutexCreate_m();
        if (!handle->mutex_h) {
            LOGE("HyThreadMutexCreate failed \n");
            break;
        }

        handle->cond_h = HyThreadCondCreate_m();
        if (!handle->cond_h) {
            LOGE("HyThreadCondCreate failed \n");
            break;
        }

        num = save_c->num;
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

    LOGE("package list create failed \n");
    HyPackageListDestroy(&handle);
    return NULL;
}
