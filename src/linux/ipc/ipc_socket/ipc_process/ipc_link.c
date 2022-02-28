/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    ipc_link.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    21/02 2022 19:17
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        21/02 2022      create the file
 * 
 *     last modified: 21/02 2022 19:17
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"

#include "ipc_link.h"


void ipc_link_msg_usr_destroy(ipc_link_msg_usr_s *ipc_msg_usr)
{
    LOGT("ipc_msg_usr: %p \n", ipc_msg_usr);
    HY_ASSERT_RET(!ipc_msg_usr);

    // LOGD("ipc link msg usr destroy, ipc_msg_usr: %p \n", ipc_msg_usr);
    HY_MEM_FREE_PP(&ipc_msg_usr);
}

ipc_link_msg_usr_s *ipc_link_msg_usr_create(void *ipc_link,
        ipc_link_msg_s *ipc_msg)
{
    LOGT("ipc_link: %p, ipc_msg: %p \n", ipc_link, ipc_msg);
    HY_ASSERT_RET_VAL(!ipc_link || !ipc_msg, NULL);

    ipc_link_msg_usr_s *ipc_msg_usr = NULL;
    do {
        ipc_msg_usr = HY_MEM_MALLOC_BREAK(ipc_link_msg_usr_s *,
                sizeof(*ipc_msg_usr));

        ipc_msg_usr->ipc_link = ipc_link;
        ipc_msg_usr->ipc_msg = ipc_msg;

        // LOGD("ipc link msg usr create, ipc_msg_usr: %p \n", ipc_msg_usr);
        return ipc_msg_usr;
    } while (0);

    LOGE("ipc link msg usr failed \n");
    return NULL;
}

void ipc_link_dump(ipc_link_s *ipc_link)
{
    LOGT("ipc_link: %p \n", ipc_link);
    HY_ASSERT_RET(!ipc_link);

    LOGI("ipc_link: %p \n",             ipc_link);
    LOGI("ipc_socket_handle: %p \n",    ipc_link->ipc_socket_handle);
    LOGI("tag: %s \n",                  ipc_link->tag);
    LOGI("ipc_name: %s \n",             HyIpcSocketGetName(ipc_link->ipc_socket_handle));
    LOGI("pid: %d \n",                  ipc_link->pid);
    LOGI("is_connect: %d \n",           ipc_link->is_connect);
    LOGI("link_type: %d \n",            ipc_link->link_type);

}

hy_s32_t ipc_link_get_fd(ipc_link_s *ipc_link)
{
    LOGT("ipc_link: %p \n", ipc_link);
    HY_ASSERT_RET_VAL(!ipc_link, -1);

    return HyIpcSocketGetFD(ipc_link->ipc_socket_handle);
}

hy_s32_t ipc_link_wait_accept(ipc_link_s *ipc_link,
        HyIpcSocketAcceptCb_t accept_cb, void *args)
{
    LOGT("ipc_link: %p, args: %p \n", ipc_link, args);
    HY_ASSERT_RET_VAL(!ipc_link, -1);

    void *ipc_socket_handle = ipc_link->ipc_socket_handle;

    if (!ipc_socket_handle) {
        LOGE("ipc_socket_handle is NULL \n");
        return -1;
    }

    return HyIpcSocketAccept(ipc_link->ipc_socket_handle, accept_cb, args);
}

hy_s32_t ipc_link_connect(ipc_link_s *ipc_link, hy_u32_t timeout_s)
{
    LOGT("ipc_link: %p, timeout_s: %d \n", ipc_link, timeout_s);
    HY_ASSERT_RET_VAL(!ipc_link, -1);

    void *ipc_socket_handle = ipc_link->ipc_socket_handle;

    if (!ipc_socket_handle) {
        LOGE("ipc_socket_handle is NULL \n");
        return -1;
    }

    if (0 != HyIpcSocketConnect(ipc_socket_handle, timeout_s)) {
        LOGE("HyIpcSocketConnect failed \n");
        return -1;
    } else {
        return 0;
    }
}

void ipc_link_set_info(ipc_link_s *ipc_link, const char *tag, pid_t pid)
{
    LOGT("ipc_link: %p, tag: %s, pid: %d \n", ipc_link, tag, pid);
    HY_ASSERT_RET(!ipc_link || !tag);

    ipc_link->pid = pid;
    HY_STRNCPY(ipc_link->tag, HY_IPC_PROCESS_IPC_NAME_LEN_MAX / 2,
            tag, HY_STRLEN(tag));
}

void ipc_link_get_info(ipc_link_s *ipc_link, HyIpcProcessInfo_s *ipc_process_info)
{
    LOGT("ipc_link: %p, ipc_process_info: %p \n", ipc_link, ipc_process_info);
    HY_ASSERT_RET(!ipc_link || !ipc_process_info);

    ipc_process_info->tag       = ipc_link->tag;
    ipc_process_info->pid       = ipc_link->pid;
    ipc_process_info->ipc_name  = HyIpcSocketGetName(ipc_link->ipc_socket_handle);
}

hy_s32_t ipc_link_parse_msg(ipc_link_s *ipc_link,
        ipc_link_parse_msg_cb_s *parse_msg_cb)
{
    ipc_link_msg_s *ipc_msg = NULL;
    pid_t pid = 0;
    HyIpcProcessInfo_s ipc_process_info;

    if (0 != ipc_link_read(ipc_link, &ipc_msg)) {
        LOGE("ipc link read failed \n");
        return -1;
    }

    switch (ipc_msg->type) {
        case IPC_LINK_MSG_TYPE_RETURN:
            LOGE("--------haha----return \n");
            break;
        case IPC_LINK_MSG_TYPE_CB:
            LOGE("--------haha----cb \n");
            break;
        case IPC_LINK_MSG_TYPE_INFO:
            pid = *(pid_t *)(ipc_msg->buf + HY_STRLEN(ipc_msg->buf) + 1);
            ipc_link_set_info(ipc_link, ipc_msg->buf, pid);

            ipc_process_info.tag        = ipc_link->tag;
            ipc_process_info.pid        = ipc_link->pid;
            ipc_process_info.ipc_name   = HyIpcSocketGetName(ipc_link->ipc_socket_handle);

            if (parse_msg_cb) {
                parse_msg_cb->parse_info_cb(&ipc_process_info,
                        HY_IPC_PROCESS_STATE_CONNECT, parse_msg_cb->args);
            }
            break;
        default:
            LOGE("error ipc_msg type\n");
            break;
    }

    if (ipc_msg) {
        HY_MEM_FREE_P(ipc_msg);
    }

    return 0;
}

hy_s32_t ipc_link_write_info(ipc_link_s *ipc_link, pid_t pid)
{
    LOGT("ipc_link: %p, pid: %d \n", ipc_link, pid);
    HY_ASSERT_RET_VAL(!ipc_link, -1);

    hy_u32_t total_len = sizeof(ipc_link_msg_s)
        + HY_IPC_PROCESS_IPC_NAME_LEN_MAX + sizeof(pid_t);
    hy_s32_t offset = 0;
    char *ipc_msg_buf = NULL;
    ipc_link_msg_s *ipc_msg = NULL;

    ipc_msg_buf = HY_MEM_MALLOC_RET_VAL(char *, total_len, -1);

    ipc_msg = (ipc_link_msg_s *)ipc_msg_buf;

    HY_MEMCPY(ipc_msg->buf + offset, ipc_link->tag, HY_STRLEN(ipc_link->tag));
    offset += HY_STRLEN(ipc_link->tag) + 1;

    HY_MEMCPY(ipc_msg->buf + offset, &pid, sizeof(pid_t));
    offset += sizeof(pid_t);

    ipc_msg->total_len  = sizeof(ipc_link_msg_s) + offset;
    ipc_msg->type       = IPC_LINK_MSG_TYPE_INFO;
    ipc_msg->thread_id  = pthread_self();
    ipc_msg->buf_len    = offset;

    return ipc_link_write(ipc_link, ipc_msg);
}

hy_s32_t ipc_link_read(ipc_link_s *ipc_link, ipc_link_msg_s **ipc_msg)
{
    LOGT("ipc_link: %p, &ipc_msg: %p \n", ipc_link, ipc_msg);
    HY_ASSERT_RET_VAL(!ipc_link || !ipc_msg, -1);

    hy_s32_t total_len = 0;
    char *ipc_link_msg_buf = NULL;

    do {
        if (HY_IPC_SOCKET_CONNECT_STATE_CONNECT
                != HyIpcSocketGetConnectState(ipc_link->ipc_socket_handle)) {
            LOGE("ipc socket disconnect \n");
            break;
        }

        if (-1 == HyIpcSocketRead(ipc_link->ipc_socket_handle,
                    &total_len, sizeof(hy_u32_t))) {
            LOGE("ipc socket read failed \n");
            break;
        }

        ipc_link_msg_buf = HY_MEM_MALLOC_BREAK(char *, total_len);
        *ipc_msg = (ipc_link_msg_s *)ipc_link_msg_buf;

        if (-1 == HyIpcSocketRead(ipc_link->ipc_socket_handle,
                    ipc_link_msg_buf, total_len)) {
            LOGE("ipc socket read failed \n");
            break;
        }

        return 0;
    } while (0);

    return -1;
}

hy_s32_t ipc_link_write(ipc_link_s *ipc_link, ipc_link_msg_s *ipc_msg)
{
    LOGT("ipc_link: %p, ipc_msg: %p \n", ipc_link, ipc_msg);
    HY_ASSERT(ipc_link);
    HY_ASSERT(ipc_msg);

    hy_s32_t ret = -1;

    do {
        if (HY_IPC_SOCKET_CONNECT_STATE_CONNECT
                != HyIpcSocketGetConnectState(ipc_link->ipc_socket_handle)) {
            LOGE("ipc socket disconnect \n");
            break;
        }

        if (-1 == HyIpcSocketWrite(ipc_link->ipc_socket_handle,
                    &ipc_msg->total_len, sizeof(hy_u32_t))) {
            LOGE("ipc socket write failed \n");
            break;
        }

        if (-1 == HyIpcSocketWrite(ipc_link->ipc_socket_handle,
                    ipc_msg, ipc_msg->total_len)) {
            LOGE("ipc socket write failed \n");
            break;
        }

        ret = 0;
    } while (0);

    free(ipc_msg);

    return (ret == 0 ? 0 : -1);
}

void ipc_link_destroy(ipc_link_s **ipc_link_pp)
{
    LOGT("&ipc_link: %p, ipc_link: %p \n", ipc_link_pp, *ipc_link_pp);
    HY_ASSERT_RET(!ipc_link_pp || !*ipc_link_pp);

    ipc_link_s *ipc_link = *ipc_link_pp;

    HyIpcSocketDestroy(&ipc_link->ipc_socket_handle);

    LOGI("ipc link destroy, ipc_link: %p \n", ipc_link);
    HY_MEM_FREE_PP(ipc_link_pp);
}

ipc_link_s *ipc_link_create(const char *name, const char *tag,
        ipc_link_type_e type, void *ipc_socket_handle)
{
    LOGT("name: %s, tag: %s, type: %d \n", name, tag, type);
    // HY_ASSERT_RET_VAL(!name || !tag, NULL);

    ipc_link_s *ipc_link = NULL;
    HyIpcSocketType_e ipc_socket_type[HY_IPC_SOCKET_TYPE_MAX] = {
        HY_IPC_SOCKET_TYPE_CLIENT, HY_IPC_SOCKET_TYPE_SERVER
    };

    do {
        ipc_link = HY_MEM_MALLOC_BREAK(ipc_link_s *, sizeof(*ipc_link));

        if (ipc_socket_handle) {
            ipc_link->ipc_socket_handle = ipc_socket_handle;
        } else {
            ipc_link->pid = getpid();
            HY_STRNCPY(ipc_link->tag, HY_IPC_PROCESS_IPC_NAME_LEN_MAX / 2,
                    tag, HY_STRLEN(tag));

            ipc_link->ipc_socket_handle = HyIpcSocketCreate_m(name,
                    ipc_socket_type[type]);
            if (!ipc_link->ipc_socket_handle) {
                LOGE("HyIpcSocketCreate_m failed \n");
                break;
            }
        }

        LOGI("ipc link create, ipc_link: %p \n", ipc_link);
        return ipc_link;
    } while (0);

    LOGI("ipc link create failed \n");
    ipc_link_destroy(&ipc_link);
    return NULL;
}
