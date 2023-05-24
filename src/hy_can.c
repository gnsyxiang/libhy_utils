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
#include "hy_string.h"
#include "hy_utils.h"
#include "hy_file.h"

#include "hy_can.h"

struct HyCan_s {
    HyCanSaveConfig_s   save_c;

    hy_u32_t            *filter_id;
    hy_u32_t            filter_id_cnt;

    hy_s32_t            fd;
};

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
        setsockopt(fd, SOL_CAN_RAW, CAN_RAW_JOIN_FILTERS,
                   &join_filter, sizeof(join_filter));
    }
    setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    return 0;
}

static hy_s32_t _can_bind_socket(HyCan_s *handle)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    handle->fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (handle->fd < 0) {
        LOGES("socket failed \n");
        return -1;
    }
    LOGI("socket can fd: %d, name: %s \n", handle->fd, handle->save_c.name);

    HY_MEMSET(&addr, sizeof(addr));
    strcpy(ifr.ifr_name, handle->save_c.name);
    ioctl(handle->fd, SIOCGIFINDEX, &ifr);
    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(handle->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        LOGES("bind error! \n");
        return -1;
    }

    return 0;
}

static void _can_deinit(const char *name)
{
    char param[64];

    HY_MEMSET(param, sizeof(param));
    snprintf(param, sizeof(param), "ip link set %s down", name);
    HyUtilsSystemCmd_m(param, 0);
}

static hy_s32_t _can_init(const char *name, HyCanSpeed_e speed)
{
    char param[64];
    hy_u32_t speed_num = 0;
    size_t i = 0;
    struct {
        HyCanSpeed_e    speed;
        hy_u32_t        speed_num;
    } speed_array[] = {
        {HY_CAN_SPEED_5K,       5 * 1000},
        {HY_CAN_SPEED_10K,      10 * 1000},
        {HY_CAN_SPEED_20K,      20 * 1000},
        {HY_CAN_SPEED_40K,      40 * 1000},
        {HY_CAN_SPEED_50K,      50 * 1000},
        {HY_CAN_SPEED_80K,      80 * 1000},
        {HY_CAN_SPEED_100K,     100 * 1000},
        {HY_CAN_SPEED_125K,     125 * 1000},
        {HY_CAN_SPEED_200K,     200 * 1000},
        {HY_CAN_SPEED_250K,     250 * 1000},
        {HY_CAN_SPEED_400K,     400 * 1000},
        {HY_CAN_SPEED_500K,     500 * 1000},
        {HY_CAN_SPEED_666K,     666 * 1000},
        {HY_CAN_SPEED_800K,     800 * 1000},
        {HY_CAN_SPEED_1000K,    1000 * 1000},
    };

    for (i = 0; i < HY_UTILS_ARRAY_CNT(speed_array); i++) {
        if (speed_array[i].speed == speed) {
            speed_num = speed_array[i].speed_num;
            break;
        }
    }

    if (i >= HY_UTILS_ARRAY_CNT(speed_array)) {
        LOGE("the speed is error \n");
        return -1;
    }

    HY_MEMSET(param, sizeof(param));
    snprintf(param, sizeof(param), "echo 4096 > /sys/class/net/can0/tx_queue_len");
    HyUtilsSystemCmd_m(param, 0);

    HY_MEMSET(param, sizeof(param));
    snprintf(param, sizeof(param), "ip link set %s down", name);
    HyUtilsSystemCmd_m(param, 0);

    HY_MEMSET(param, sizeof(param));
    snprintf(param, sizeof(param), "ip link set %s type can bitrate %d", name, speed_num);
    HyUtilsSystemCmd_m(param, 0);

    HY_MEMSET(param, sizeof(param));
    snprintf(param, sizeof(param), "ip link set %s up", name);
    HyUtilsSystemCmd_m(param, 0);

    return 0;
}

hy_s32_t HyCanWrite(HyCan_s *handle, struct can_frame *tx_frame)
{
    HY_ASSERT(handle);
    HY_ASSERT(tx_frame);

    hy_s32_t ret;

    ret = write(handle->fd, tx_frame, sizeof(struct can_frame));
    if (ret < 0 && errno == EINTR) {
        LOGW("write failed, errno is EINTR \n");
        return 0;
    } else if (ret == -1) {
        LOGES("fd close, fd: %d \n", handle->fd);
        return -1;
    } else {
        return ret;
    }
}

hy_s32_t HyCanWriteBuf(HyCan_s *handle, hy_u32_t can_id, char *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    struct can_frame tx_frame;
    hy_s32_t shang = len / 8;
    hy_s32_t yushu = len % 8;
    hy_s32_t ret = 0;

    tx_frame.can_id = can_id;
    tx_frame.can_dlc = 8;

    // @fixme 考虑写失败情况
    for (hy_s32_t i = 0; i < shang; ++i) {
        memcpy(tx_frame.data, buf + 8 * i, 8);
        ret = write(handle->fd, &tx_frame, sizeof(struct can_frame));
        if (ret < 0 && errno == EINTR) {
            LOGE("write failed, errno is EINTR, i: %d \n", i);
            return 0;
        } else if (ret == -1) {
            if (errno == 105) {
                usleep(500);
                ret = write(handle->fd, &tx_frame, sizeof(struct can_frame));
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
        HyCanWrite(handle, &tx_frame);
    }

    return 0;
}

hy_s32_t HyCanRead(HyCan_s *handle, struct can_frame *rx_frame)
{
    HY_ASSERT(handle);
    HY_ASSERT(rx_frame);

    hy_s32_t ret;

    do {
        ret = HyFileRead(handle->fd, rx_frame, sizeof(*rx_frame));
    } while(ret == 0);

    return ret;
}

hy_s32_t HyCanReadTimeout(HyCan_s *handle, struct can_frame *rx_frame, hy_u32_t ms)
{
    HY_ASSERT(handle);
    HY_ASSERT(rx_frame);

    return HyFileReadTimeout(handle->fd, rx_frame, sizeof(*rx_frame), ms);
}

void HyCanDestroy(HyCan_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyCan_s *handle = *handle_pp;

    LOGI("close can fd: %d, name: %s \n", handle->fd, handle->save_c.name);

    close(handle->fd);
    handle->fd = -1;

    _can_deinit(handle->save_c.name);

    LOGI("can destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(&handle);
}

HyCan_s *HyCanCreate(HyCanConfig_s *can_c)
{
    HY_ASSERT_RET_VAL(!can_c, NULL);
    HyCan_s *handle = NULL;

    do {
        handle = HY_MEM_CALLOC_BREAK(HyCan_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &can_c->save_c, sizeof(handle->save_c));

        HyCanSaveConfig_s *save_c = &can_c->save_c;

        handle->filter_id = HY_MEM_CALLOC_BREAK(hy_u32_t *, can_c->filter_id_cnt);
        HY_MEMCPY(handle->filter_id, can_c->filter_id,
                  sizeof(hy_u32_t) * can_c->filter_id_cnt);
        handle->filter_id_cnt = can_c->filter_id_cnt;

        if (0 != _can_init(save_c->name, can_c->speed)) {
            LOGE("_can_init failed \n");
            break;
        }

        if (0 != _can_bind_socket(handle)) {
            LOGE("can bind socket failed \n");
            break;
        }

        _can_set_recv_filter(handle->fd, handle->filter_id,
                             handle->filter_id_cnt, save_c->filter);

        LOGI("can create, handle: %p \n", handle);
        return handle;
    } while (0);

    LOGE("can create failed \n");
    HyCanDestroy(&handle);
    return NULL;
}
