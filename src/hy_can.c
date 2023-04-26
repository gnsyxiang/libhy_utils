/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_can.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    18/03 2023 15:06
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        18/03 2023      create the file
 * 
 *     last modified: 18/03 2023 15:06
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include <net/if.h>
#include <sys/ioctl.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_mem.h"

#include "hy_can.h"

typedef struct {
    HyCanSaveConfig_s   save_c;

    const char          *name;
    hy_s32_t            fd;
} _can_context_s;

static hy_s32_t _can_set_recv_filter(hy_s32_t fd, const hy_u32_t *can_id,
        hy_u32_t cnt, HyCanFilter_e type)
{
    if (fd <= 0) {
        LOGE("the fd: %d <= 0 \n", fd);
        return -1;
    }

    if (NULL == can_id) {
        LOGI("can no filtering \n");
        setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
        return 0;
    }

    struct can_filter rfilter[cnt];
    HY_MEMSET(rfilter, sizeof(rfilter));

    for (hy_u32_t i = 0; i < cnt; ++i) {
        if (can_id[i]) {
            LOGI("set can id: %x \n", can_id[i]);

            if (type == HY_CAN_FILTER_PASS) {
                rfilter[i].can_id = can_id[i];
            } else {
                rfilter[i].can_id = can_id[i] | CAN_INV_FILTER;
            }
            rfilter[i].can_mask = CAN_ERR_MASK;
        } else {
            LOGE("the can id array is error \n");
            return -1;
        }
    }

    if (type & HY_CAN_FILTER_REJECT) {
        hy_s32_t join_filter = 1;
        setsockopt(fd, SOL_CAN_RAW, CAN_RAW_JOIN_FILTERS, &join_filter, sizeof(join_filter));
    }
    setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    return 0;
}

static hy_s32_t _can_bind_socket(_can_context_s *context, const char *name)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    context->fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (context->fd < 0) {
        LOGES("socket failed \n");
        return -1;
    }
    LOGI("socket can fd: %d, name: %s \n", context->fd, name);

    HY_MEMSET(&addr, sizeof(addr));
    strcpy(ifr.ifr_name, name);
    ioctl(context->fd, SIOCGIFINDEX, &ifr);
    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(context->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        LOGES("bind error! \n");
        return -1;
    }

    return 0;
}

static void _can_init(const char *name, hy_u32_t speed)
{
    char param[64];

    HY_MEMSET(param, sizeof(param));
    snprintf(param, sizeof(param), "ifconfig %s down", name);
    system(param);

    HY_MEMSET(param, sizeof(param));
    snprintf(param, sizeof(param), "ip link set %s type can bitrate %d triple-sampling on", name, speed);
    system(param);

    HY_MEMSET(param, sizeof(param));
    snprintf(param, sizeof(param), "ifconfig %s up", name);
    system(param);
}

hy_s32_t HyCanGetFd(void *handle)
{
    HY_ASSERT(handle);
    _can_context_s *context = handle;

    return context->fd;
}

hy_s32_t HyCanWrite(void *handle, struct can_frame *tx_frame)
{
    HY_ASSERT(handle);
    HY_ASSERT(tx_frame);

    hy_s32_t ret;
    _can_context_s *context = handle;

    ret = write(context->fd, tx_frame, sizeof(struct can_frame));
    if (ret < 0 && errno == EINTR) {
        LOGW("write failed, errno is EINTR \n");
        return 0;
    } else if (ret == -1) {
        LOGES("fd close, fd: %d \n", context->fd);
        return -1;
    } else {
        return ret;
    }
}

hy_s32_t HyCanWriteBuf(void *handle, hy_u32_t can_id, char *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    _can_context_s *context = handle;
    struct can_frame tx_frame;
    hy_s32_t shang = len / 8;
    hy_s32_t yushu = len % 8;
    hy_s32_t ret = 0;

    tx_frame.can_id = can_id;
    tx_frame.can_dlc = 8;

    // @fixme 考虑写失败情况
    for (hy_s32_t i = 0; i < shang; ++i) {
        memcpy(tx_frame.data, buf + 8 * i, 8);
        ret = write(context->fd, &tx_frame, sizeof(struct can_frame));
        if (ret < 0 && errno == EINTR) {
            LOGW("write failed, errno is EINTR \n");
            return 0;
        } else if (ret == -1) {
            LOGW("write failed first, i: %d \n", i);
            if (errno == 105) {
                usleep(500);
                ret = write(context->fd, &tx_frame, sizeof(struct can_frame));
                if (ret == -1) {
                    LOGES("write failed, i: %d \n", i);
                }
            } else {
                return -1;
            }
        }
    }

    if (yushu > 0) {
        tx_frame.can_dlc = yushu;
        memcpy(tx_frame.data, buf + 8 * shang, yushu);
        HyCanWrite(context, &tx_frame);
    }

    return 0;
}

hy_s32_t HyCanRead(void *handle, struct can_frame *rx_frame)
{
    HY_ASSERT_RET_VAL(!handle, -1);
    HY_ASSERT_RET_VAL(!rx_frame, -1);

    hy_s32_t ret;
    _can_context_s *context = handle;

    ret = read(context->fd, rx_frame, sizeof(struct can_frame));
    if (ret < 0) {
        if (EINTR == errno || EAGAIN == errno || EWOULDBLOCK == errno) {
            LOGW("read failed, errno is EINTR/EAGAIN/EWOULDBLOCK \n");
            ret = 0;
        } else {
            LOGE("read failed \n");
            return -1;
        }
    } else if (ret == 0) {
        LOGE("fd close, fd: %d \n", context->fd);
    }

    return ret;
}

void HyCanDestroy(void **handle)
{
    HY_ASSERT_RET(!handle);
    HY_ASSERT_RET(!*handle);
    _can_context_s *context = *handle;

    LOGI("close can fd: %d, name: %s \n", context->fd, context->name);

    close(context->fd);

    char param[64];

    HY_MEMSET(param, sizeof(param));
    snprintf(param, sizeof(param), "ifconfig %s down", context->name);
    system(param);

    LOGI("can destroy, context: %p, fd: %d \n", context, context->fd);
    HY_MEM_FREE_PP(&context);
}

void *HyCanCreate(HyCanConfig_s *can_c)
{
    HY_ASSERT_RET_VAL(!can_c, NULL);

    _can_context_s *context = NULL;
    do {
        context = HY_MEM_CALLOC_BREAK(_can_context_s *, sizeof(*context));
        if (can_c->save_c.can_id_cnt > HY_CAN_ID_MAX) {
            LOGE("can id cnt is too long, "
                 "please fix  HY_CAN_ID_MAX in hy_can.h \n");
            break;
        }

        memcpy(&context->save_c,
               &can_c->save_c, sizeof(context->save_c));

        _can_init(can_c->name, can_c->speed);

        if (0 != _can_bind_socket(context, can_c->name)) {
            LOGE("can bind socket failed \n");
            break;
        }

        context->name = can_c->name;
        HyCanSaveConfig_s *save_c = &can_c->save_c;

        _can_set_recv_filter(context->fd,
                             save_c->can_id, save_c->can_id_cnt, save_c->filter);

        LOGI("can create, context: %p, fd: %d \n",
             context, context->fd);
        return context;
    } while (0);

    LOGE("can create failed \n");
    HyCanDestroy((void **)&context);
    return NULL;
}

