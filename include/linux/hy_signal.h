/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_signal.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    25/10 2021 19:00
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        25/10 2021      create the file
 * 
 *     last modified: 25/10 2021 19:00
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_SIGNAL_H_
#define __LIBHY_UTILS_INCLUDE_HY_SIGNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <signal.h>

#include <hy_os_type/hy_type.h>

#define HY_SIGNAL_NUM_MAX_32   (64)             ///< 系统信号最大值

/**
 * @brief 配置结构体
 */
typedef struct {
    const char  *app_name;                      ///< app名字
    const char  *coredump_path;                 ///< 生成coredump路径

    void        (*user_cb)(void *args);         ///< 用户关心信号的回调函数
    void        (*error_cb)(void *args);        ///< 程序运行错误信号的回调函数
    void        *args;                          ///< 上层传递的参数
} HySignalSaveConfig_t;

/**
 * @brief 配置结构体
 */
typedef struct {
    HySignalSaveConfig_t    save_c;                             ///< 配置参数

    hy_u8_t                 user_num[HY_SIGNAL_NUM_MAX_32];     ///< 注册用户信号
    hy_u8_t                 error_num[HY_SIGNAL_NUM_MAX_32];    ///< 注册系统错误信号
} HySignalConfig_t;

/**
 * @brief 创建信号处理模块
 *
 * @param config 配置参数
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HySignalCreate(HySignalConfig_t *signal_c);

/**
 * @brief 创建信号处理模块宏
 *
 * @param _app_name app名字
 * @param _core_path 生成coredump路径
 * @param _error_cb 程序运行错误信号的回调函数
 * @param _user_cb 用户关心信号的回调函数
 * @param args 上层传递的参数
 *
 * @return 成功返回0，失败返回-1
 */
#define HySignalCreate_m(_app_name, _core_path, _error_cb, _user_cb, args)  \
    ({                                                                      \
        hy_u8_t signal_error[HY_SIGNAL_NUM_MAX_32] = {                      \
            SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGFPE,                      \
            SIGSEGV, SIGBUS, SIGSYS, SIGXCPU, SIGXFSZ,                      \
        };                                                                  \
        hy_u8_t signal_user[HY_SIGNAL_NUM_MAX_32] = {                       \
            SIGINT, SIGTERM, SIGUSR1, SIGUSR2,                              \
        };                                                                  \
        HySignalConfig_t signal_c;                                          \
        HY_MEMSET(&signal_c, sizeof(signal_c));                             \
        HY_MEMCPY(signal_c.error_num, signal_error, sizeof(signal_error));  \
        HY_MEMCPY(signal_c.user_num, signal_user, sizeof(signal_user));     \
        signal_c.save_c.app_name           = _app_name;                     \
        signal_c.save_c.coredump_path     = _core_path;                     \
        signal_c.save_c.error_cb          = _error_cb;                      \
        signal_c.save_c.user_cb           = _user_cb;                       \
        signal_c.save_c.args              = _args;                          \
                                                                            \
        HySignalCreate(&signal_c);                                          \
    })

/**
 * @brief 销毁信号处理模块
 */
void HySignalDestroy(void);

#ifdef __cplusplus
}
#endif

#endif

