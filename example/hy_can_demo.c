/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_can_demo.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    25/10 2021 19:56
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        25/10 2021      create the file
 * 
 *     last modified: 25/10 2021 19:56
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "hy_type.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_signal.h"
#include "hy_module.h"
#include "hy_utils.h"
#include "hy_time.h"
#include "hy_can.h"

#define _APP_NAME "hy_can_demo"
#define _CAN_NAME "can0"

typedef struct {
    hy_s32_t    is_exit;

    void        *can_handle;
} _main_can_s;

static void _can_test_write(_main_can_s *context)
{
    char can_buf[128];
    for (unsigned int i = 0; i < sizeof(can_buf) / sizeof(can_buf[0]); ++i) {
        can_buf[i] = i;
    }
    unsigned long long start = HyTimeGetUTCUs();
    LOGI("start time: %lld \n", start);
    HyCanWriteBuf(context->can_handle, 0x110, can_buf,
                  sizeof(can_buf) / sizeof(can_buf[0]));
    unsigned long long end = HyTimeGetUTCUs();
    LOGI("end time: %lld, speed time: %lld \n", end, end - start);
}

static void _signal_error_cb(void *args)
{
    LOGE("------error cb\n");

    _main_can_s *context = args;
    context->is_exit = 1;
}

static void _signal_user_cb(void *args)
{
    LOGW("------user cb\n");

    _main_can_s *context = args;
    context->is_exit = 1;
}

static void _bool_module_destroy(void)
{
    HyModuleDestroyBool_s bool_module[] = {
        {"signal",          HySignalDestroy },
        {"log",             HyLogDeInit     },
    };

    HY_MODULE_RUN_DESTROY_BOOL(bool_module);
}

static hy_s32_t _bool_module_create(_main_can_s *context)
{
    HyLogConfig_s log_c;
    HY_MEMSET(&log_c, sizeof(log_c));
    log_c.config_file               = "../res/hy_log/zlog.conf";
    log_c.fifo_len                  = 10 * 1024;
    log_c.save_c.level              = HY_LOG_LEVEL_TRACE;
    log_c.save_c.output_format      = HY_LOG_OUTFORMAT_ALL;

    int8_t signal_error_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,
        SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,
    };

    int8_t signal_user_num[HY_SIGNAL_NUM_MAX_32] = {
        SIGINT, SIGTERM, SIGUSR1, SIGUSR2,
    };

    HySignalConfig_t signal_c;
    HY_MEMSET(&signal_c, sizeof(signal_c));
    HY_MEMCPY(signal_c.error_num, signal_error_num, sizeof(signal_error_num));
    HY_MEMCPY(signal_c.user_num, signal_user_num, sizeof(signal_user_num));
    signal_c.save_c.app_name        = _APP_NAME;
    signal_c.save_c.coredump_path   = "./";
    signal_c.save_c.error_cb        = _signal_error_cb;
    signal_c.save_c.user_cb         = _signal_user_cb;
    signal_c.save_c.args            = context;

    HyModuleCreateBool_s bool_module[] = {
        {"log",         &log_c,         (HyModuleCreateBoolCb_t)HyLogInit,          HyLogDeInit},
        {"signal",      &signal_c,      (HyModuleCreateBoolCb_t)HySignalCreate,     HySignalDestroy},
    };

    HY_MODULE_RUN_CREATE_BOOL(bool_module);
}

static void _handle_module_destroy(_main_can_s *context)
{
    // note: 增加或删除要同步到HyModuleCreateHandle_s中
    HyModuleDestroyHandle_s module[] = {
        {"can0",        &context->can_handle,           (HyModuleDestroyHandleCb_t)HyCanDestroy},
    };

    HY_MODULE_RUN_DESTROY_HANDLE(module);
}

static hy_s32_t _handle_module_create(_main_can_s *context)
{
    HyCanConfig_s can_c;
    hy_u32_t can_id[] = {0x110, 0x10, 0x2};

    HY_MEMSET(&can_c, sizeof(can_c));
    can_c.speed = HY_CAN_SPEED_200K;
    can_c.filter_id = can_id;
    can_c.filter_id_cnt = sizeof(can_id) / sizeof(can_id[0]);
    can_c.save_c.name = _CAN_NAME;
    can_c.save_c.filter = HY_CAN_FILTER_PASS;

    // note: 增加或删除要同步到HyModuleDestroyHandle_s中
    HyModuleCreateHandle_s module[] = {
        {"can0",        &context->can_handle,           &can_c,             (HyModuleCreateHandleCb_t)HyCanCreate,      (HyModuleDestroyHandleCb_t)HyCanDestroy},
    };

    HY_MODULE_RUN_CREATE_HANDLE(module);
}

int main(int argc, char *argv[])
{
    _main_can_s *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(_main_can_s *, sizeof(*context));

        if (0 != _bool_module_create(context)) {
            printf("_bool_module_create failed \n");
            break;
        }

        if (0 != _handle_module_create(context)) {
            LOGE("_handle_module_create failed \n");
            break;
        }

        LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

        _can_test_write(context);

        struct can_frame rx_frame;
        while (!context->is_exit) {

            memset(&rx_frame, 0, sizeof(rx_frame));
            HyCanRead(context->can_handle, &rx_frame);
            HyCanWrite(context->can_handle, &rx_frame);

            char buf[128];
            int ret = 0;
            static int can_cnt = 0;

            can_cnt++;

            memset(buf, '\0', sizeof(buf));
            ret = snprintf(buf + ret, sizeof(buf) - ret, "cnt: %d", can_cnt);
            ret += snprintf(buf + ret, sizeof(buf) - ret, ", dlc: %d", rx_frame.can_dlc);
            for (int i = 0; i < rx_frame.can_dlc; ++i) {
                ret += snprintf(buf + ret, sizeof(buf) - ret, ", 0x%0x", rx_frame.data[i]);
            }
            LOGI("%s \n", buf);

            usleep(33 * 1000);
        }
    } while (0);

    _handle_module_destroy(context);
    _bool_module_destroy();
    HY_MEM_FREE_PP(&context);

    return 0;
}

