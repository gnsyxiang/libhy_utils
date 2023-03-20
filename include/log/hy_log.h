/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_log.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    29/10 2021 20:29
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        29/10 2021      create the file
 * 
 *     last modified: 29/10 2021 20:29
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_LOG_H_
#define __LIBHY_UTILS_INCLUDE_HY_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <pthread.h>

#include <sys/syscall.h>      /* Definition of SYS_* constants */

#include "hy_type.h"

/**
 * @brief 打印等级定义
 *
 * @note 数字越小越紧急
 */
typedef enum {
    HY_LOG_LEVEL_FATAL,                     ///< 致命错误，立刻停止程序
    HY_LOG_LEVEL_ERROR,                     ///< 错误，停止程序
    HY_LOG_LEVEL_WARN,                      ///< 警告
    HY_LOG_LEVEL_INFO,                      ///< 追踪，记录程序运行到哪里
    HY_LOG_LEVEL_DEBUG,                     ///< 调试程序相关打印
    HY_LOG_LEVEL_TRACE,                     ///< 程序打点调试

    HY_LOG_LEVEL_MAX = 0xffffffff,
} HyLogLevel_e;

/**
 * @brief log相关信息
 */
typedef struct {
    HyLogLevel_e        level;              ///< 打印等级
    char                *err_str;           ///< 错误信息，由strerror(errno)提供
    const char          *file;              ///< 文件名，去掉了路径
    const char          *func;              ///< 函数名
    hy_u32_t            line;               ///< 行号
    pthread_t           tid;                ///< 线程id
    long                pid;                ///< 进程id

    const char          *fmt;               ///< 用户格式
    va_list             *str_args;          ///< 用户信息
} HyLogAddiInfo_s;

/**
 * @brief 配置log输出的格式
 */
typedef enum {
    HY_LOG_OUTPUT_FORMAT_COLOR          = (0x1 << 0),   ///< 颜色输出
    HY_LOG_OUTPUT_FORMAT_LEVEL_INFO     = (0x1 << 1),   ///< 等级提示字母
    HY_LOG_OUTPUT_FORMAT_TIME           = (0x1 << 2),   ///< 时间输出
    HY_LOG_OUTPUT_FORMAT_PID_ID         = (0x1 << 3),   ///< 进程线程id输出
    HY_LOG_OUTPUT_FORMAT_FUNC_LINE      = (0x1 << 4),   ///< 函数行号输出
    HY_LOG_OUTPUT_FORMAT_USR_MSG        = (0x1 << 5),   ///< 函数行号输出
    HY_LOG_OUTPUT_FORMAT_COLOR_RESET    = (0x1 << 6),   ///< 颜色输出恢复

    HY_LOG_OUTPUT_FORMAT_MAX            = 0xffffffff,
} HyLogOutputFormat_e;

/**
 * @brief 默认配置，输出所有格式
 */
#define HY_LOG_OUTFORMAT_ALL                \
(HY_LOG_OUTPUT_FORMAT_COLOR                 \
    | HY_LOG_OUTPUT_FORMAT_LEVEL_INFO       \
    | HY_LOG_OUTPUT_FORMAT_TIME             \
    | HY_LOG_OUTPUT_FORMAT_PID_ID           \
    | HY_LOG_OUTPUT_FORMAT_FUNC_LINE        \
    | HY_LOG_OUTPUT_FORMAT_USR_MSG          \
    | HY_LOG_OUTPUT_FORMAT_COLOR_RESET)

/**
 * @brief 默认配置中去除颜色格式
 */
#define HY_LOG_OUTFORMAT_NO_COLOR           \
(HY_LOG_OUTPUT_FORMAT_LEVEL_INFO            \
    | HY_LOG_OUTPUT_FORMAT_TIME             \
    | HY_LOG_OUTPUT_FORMAT_PID_ID           \
    | HY_LOG_OUTPUT_FORMAT_FUNC_LINE        \
    | HY_LOG_OUTPUT_FORMAT_USR_MSG)

/**
 * @brief 配置参数
 */
typedef struct {
    HyLogLevel_e        level;              ///< 打印等级

    hy_u32_t            output_format;      ///< log输出格式
} HyLogSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyLogSaveConfig_s   save_c;             ///< 配置参数

    hy_u32_t            fifo_len;           ///< fifo大小，异步方式用于保存log
} HyLogConfig_s;

/**
 * @brief 初始化log模块
 *
 * @param log_c 配置参数
 *
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyLogInit(HyLogConfig_s *log_c);

/**
 * @brief 初始化log模块
 *
 * @param _fifo_len fifo大小
 * @param _level 等级
 * @param _enable_color 是否颜色输出
 * @param _enable_time 是否时间输出
 * @param _enable_pid_id 是否进程线程id输出
 * @param _enable_func_line 是否函数行号输出
 *
 * @return 成功返回0，失败返回-1
 */
#define HyLogInit_m(_fifo_len, _mode, _level, _output_format)   \
({                                                              \
    HyLogConfig_s log_c;                                        \
    HY_MEMSET(&log_c, sizeof(log_c));                           \
    log_c.fifo_len                  = _fifo_len;                \
    log_c.save_c.level              = _level;                   \
    log_c.save_c.output_format      = _output_format;           \
    HyLogInit(&log_c);                                          \
})

/**
 * @brief 销毁log模块
 */
void HyLogDeInit(void);

/**
 * @brief 获取打印等级
 *
 * @return 当前的打印等级
 */
HyLogLevel_e HyLogLevelGet(void);

/**
 * @brief 设置打印等级
 *
 * @param level 新的打印等级
 */
void HyLogLevelSet(HyLogLevel_e level);

/**
 * @brief log函数
 *
 * @param level 打印等级
 * @param file 所在的文件
 * @param func 所在的函数
 * @param line 所在的行号
 * @param fmt 格式
 * @param ... 参数
 */
void HyLogWrite(HyLogAddiInfo_s *addi_info, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

/*
 * 1，去掉文件路径，只获取文件名
 *  1.1，使用strrchr函数，包含头文件#include <string.h>
 *      #define HY_STRRCHR_FILE (strrchr(__FILE__, '/'))
 *      #define HY_FILENAME     (HY_STRRCHR_FILE ? (HY_STRRCHR_FILE + 1) : __FILE__)
 *  1.2，使用basename函数，包含头文件#include <libgen.h>
 *      basename(__FILE__)
 */
#define HY_STRRCHR_FILE (strrchr(__FILE__, '/'))
#define HY_FILENAME     (HY_STRRCHR_FILE ? (HY_STRRCHR_FILE + 1) : __FILE__)

#define LOG(_level, _err_str, fmt, ...)                     \
do {                                                        \
    if (HyLogLevelGet() >= _level) {                        \
        HyLogAddiInfo_s addi_info;                          \
        addi_info.level     = _level;                       \
        addi_info.err_str   = _err_str;                     \
        addi_info.file      = HY_FILENAME;                  \
        addi_info.func      = __func__;                     \
        addi_info.line      = __LINE__;                     \
        addi_info.tid       = pthread_self();               \
        addi_info.pid       = syscall(SYS_gettid);          \
        HyLogWrite(&addi_info, fmt, ##__VA_ARGS__);         \
    }                                                       \
} while (0)


#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define LOGF(fmt, ...)  LOG(HY_LOG_LEVEL_FATAL, strerror(errno), fmt, ##__VA_ARGS__)
#   define LOGES(fmt, ...) LOG(HY_LOG_LEVEL_ERROR, strerror(errno), fmt, ##__VA_ARGS__)
#   define LOGE(fmt, ...)  LOG(HY_LOG_LEVEL_ERROR, NULL,            fmt, ##__VA_ARGS__)
#   define LOGW(fmt, ...)  LOG(HY_LOG_LEVEL_WARN,  NULL,            fmt, ##__VA_ARGS__)
#   define LOGI(fmt, ...)  LOG(HY_LOG_LEVEL_INFO,  NULL,            fmt, ##__VA_ARGS__)
#   define LOGD(fmt, ...)  LOG(HY_LOG_LEVEL_DEBUG, NULL,            fmt, ##__VA_ARGS__)
#   define LOGT(fmt, ...)  LOG(HY_LOG_LEVEL_TRACE, NULL,            fmt, ##__VA_ARGS__)
#else
#   define LOGF(fmt, args...)  LOG(HY_LOG_LEVEL_FATAL, strerror(errno), fmt, ##args)
#   define LOGES(fmt, args...) LOG(HY_LOG_LEVEL_ERROR, strerror(errno), fmt, ##args)
#   define LOGE(fmt, args...)  LOG(HY_LOG_LEVEL_ERROR, NULL,            fmt, ##args)
#   define LOGW(fmt, args...)  LOG(HY_LOG_LEVEL_WARN,  NULL,            fmt, ##args)
#   define LOGI(fmt, args...)  LOG(HY_LOG_LEVEL_INFO,  NULL,            fmt, ##args)
#   define LOGD(fmt, args...)  LOG(HY_LOG_LEVEL_DEBUG, NULL,            fmt, ##args)
#   define LOGT(fmt, args...)  LOG(HY_LOG_LEVEL_TRACE, NULL,            fmt, ##args)
#endif

#ifdef __cplusplus
}
#endif

#endif

