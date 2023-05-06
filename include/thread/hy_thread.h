/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_thread.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 08:26
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 08:26
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_THREAD_H_
#define __LIBHY_UTILS_INCLUDE_HY_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include "hy_type.h"

#define HY_THREAD_NAME_LEN_MAX  (16)

/**
 * @brief 线程调度策略
 */
typedef enum {
    HY_THREAD_POLICY_SCHED_OTHER,               ///< 一般模式
    HY_THREAD_POLICY_SCHED_FIFO,                ///< 队列加时间片轮询
    HY_THREAD_POLICY_SCHED_RR,                  ///< 抢占式调度

    HY_THREAD_POLICY_MAX,
} HyThreadPolicy_e;

/**
 * @brief 线程退出方式
 */
typedef enum {
    HY_THREAD_DESTROY_MODE_GRACE,               ///< 优雅退出，等待线程执行完
    HY_THREAD_DESTROY_MODE_FORCE,               ///< 强制退出，等待一定时间(2s)后强制退出

    HY_THREAD_DESTROY_MODE_MAX,
} HyThreadDestroyMode_e;

/**
 * @brief 线程是否分离
 */
typedef enum {
    HY_THREAD_DETACH_MODE_NO,                   ///< 非分离属性
    HY_THREAD_DETACH_MODE_YES,                  ///< 分离属性

    HY_THREAD_DETACH_MAX,
} HyThreadDetachMode_e;

/**
 * @brief 线程回调函数
 *
 * @param args 上层传递参数
 *
 * @return 返回0，线程继续运行，否则线程退出
 *
 * @note 创建线程里面有while循环，
 *       也可以在回调函数中增加while循环，但是要控制好退出
 */
typedef hy_s32_t (*HyThreadLoopCb_t)(void *args);

/**
 * @brief 线程私有数据销毁函数回调
 *
 * @param args 私有数据
 */
typedef void (*HyThreadKeyDestroyCb_t)(void *args);

/**
 * @brief 模块配置参数
 */
typedef struct {
    char                    name[HY_THREAD_NAME_LEN_MAX];   ///< 线程名字

    HyThreadLoopCb_t        thread_loop_cb;                 ///< 线程执行函数
    void                    *args;                          ///< 上层传递参数

    HyThreadDestroyMode_e   destroy_mode;                   ///< 线程退出方式
    HyThreadDetachMode_e    detach_mode;                    ///< 线程是否分离
    HyThreadPolicy_e        policy;                         ///< 调度策略，实时线程执行必须有root权限
    hy_u32_t                priority;                       ///< 线程优先级，只有实施线程才能设置优先级
} HyThreadSaveConfig_s;

/**
 * @brief 模块配置参数
 */
typedef struct {
    HyThreadSaveConfig_s    save_c;                         ///< 模块配置参数
} HyThreadConfig_s;

typedef struct HyThread_s HyThread_s;

/**
 * @brief 创建线程
 *
 * @param thread_c 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
HyThread_s *HyThreadCreate(HyThreadConfig_s *thread_c);

#define HyThreadCreate_m(_name, _thread_loop_cb, _args)                 \
({                                                                      \
    HyThreadConfig_s thread_c;                                          \
    HY_MEMSET(&thread_c, sizeof(thread_c));                             \
    thread_c.save_c.thread_loop_cb      = _thread_loop_cb;              \
    thread_c.save_c.args                = _args;                        \
    HY_STRNCPY(thread_c.save_c.name, HY_THREAD_NAME_LEN_MAX,            \
               _name, HY_STRLEN(_name));                                \
    HyThreadCreate(&thread_c);                                          \
})

/**
 * @brief 销毁线程
 *
 * @param handle_pp 线程句柄的地址(二级指针)
 */
void HyThreadDestroy(HyThread_s **handle_pp);

/**
 * @brief 设置线程私有数据
 *
 * @param handle 句柄
 * @param key 私有数据
 * @destroy_cb 私有数据销毁函数
 *
 * @return 成功返回0，失败返回-1
 *
 * @note 使用线程私有数据的好处是，多个线程同时使用同一个名字，但是内容不一样
 */
hy_s32_t HyThreadKeySet(HyThread_s *handle,
                        void *key, HyThreadKeyDestroyCb_t destroy_cb);

/**
 * @brief 获取线程的私有数据
 *
 * @param handle 句柄
 *
 * @return 成功返回私有数据的地址，失败返回NULL
 */
void *HyThreadKeyGet(HyThread_s *handle);

/**
 * @brief 获取线程名字
 *
 * @param handle 句柄
 *
 * @return 返回线程名字
 */
const char *HyThreadGetName(HyThread_s *handle);

/**
 * @brief 获取线程id
 *
 * @param handle 句柄
 *
 * @return 返回id(pthread线程库维护的, 进程级别)
 */
pthread_t HyThreadGetId(HyThread_s *handle);

/**
 * @brief 获取线程调度策略
 *
 * @param handle 句柄
 * @return 该线程的调度策略
 */
HyThreadPolicy_e HyThreadGetPolicy(HyThread_s *handle);

/**
 * @brief 获取线程优先级
 *
 * @param handle 句柄
 * @return 返回线程的优先级
 */
hy_u32_t HyThreadGetPriority(HyThread_s *handle);

/**
 * @brief 设置cpu到指定核
 *
 * @param cpu_index cpu序号，从0开始
 * @return 成功返回0，失败返回-1
 */
hy_s32_t HyThreadAttachCPU(hy_s32_t cpu_index);

#ifdef __cplusplus
}
#endif

#endif

