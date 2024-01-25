/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_at_utils.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    23/10 2020 16:56
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        23/10 2020      create the file
 * 
 *     last modified: 23/10 2020 16:56
 */
#ifndef __LIBUTILS_INCLUDE_HY_AT_UTILS_H_
#define __LIBUTILS_INCLUDE_HY_AT_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_os_type/hy_type.h>

#include "hy_list.h"
#include "hy_utils.h"

enum {
    AT_STATE_IDLE,
    AT_STATE_READ,
    AT_STATE_READ_HEAD,
    AT_STATE_READ_READY,
    AT_STATE_READ_READY_1,
    AT_STATE_READ_READY_2,
    AT_STATE_READ_OK,
    AT_STATE_READ_CONNECT_OK,
    AT_STATE_READ_SIGNAL_CSQ,
    AT_STATE_READ_SIGNAL_CWJAP_DEF,
    AT_STATE_READ_IPD_DATA,
    AT_STATE_READ_FAILD,
    AT_STATE_READ_ERROR,
    AT_STATE_READ_ERROR_ALREADY_CONNECTED,
    AT_STATE_READ_BUSY_S,
    AT_STATE_READ_SEND,
    AT_STATE_READ_CLOSED,
    AT_STATE_READ_MAX,
    AT_STATE_WRITE,
    AT_STATE_WRITE_CMD,
    AT_STATE_WRITE_CMD_WAIT,
    AT_STATE_WRITE_DATA,
    AT_STATE_WRITE_MAX,
};

/**
 * @brief 捕获字符串结构体
 *
 * @param state: 被捕获字符串的状态
 * @param catch_str: 被捕获字符串
 */
typedef struct {
    hy_u8_t     state;
    char        *catch_str;
} AtCatchStr_t;

/**
 * @brief at指令
 *
 * @param cmd: at模块支持的指令
 * @param data: 该指令所携带的数据
 * @param data_len: 携带数据的长度
 * @param wait_time: 该指令发出后所等待的时间
 * @param retry: 该指令重试的次数
 */
typedef struct {
    char        *cmd;
    char        *data;
    hy_u16_t    data_len;
    hy_u16_t    wait_time;
    hy_u8_t     retry;
} AtCmd_t;

/**
 * @brief 发送数据的回调函数
 *
 * @param buf: 发送的数据
 * @param len: 数据的长度
 * @param args: 传递的参数
 *
 * @note: 框架本身不能发送数据，需要外界提供这种能力
 */
typedef void (*AtUtilsWriteCb_t)(const char *buf, hy_u32_t len, void *args);

/**
 * @brief 帧的回调函数
 *
 * @param type: 返回帧的类型
 * @param buf: 返回帧的数据
 * @param len: 数据的长度
 * @param args: 传递的参数
 */
typedef void (*AtUtilsHandleFrameCb_t)(hy_u8_t type, char **buf, hy_u32_t len, void *args);

/**
 * @brief 回调函数结构体
 * @param write_cb: 发送数据的回调函数
 * @param handle_frame_cb: 帧的回调函数
 * @param args: 传递的参数
 */
typedef struct {
    AtUtilsWriteCb_t        write_cb;
    AtUtilsHandleFrameCb_t  handle_frame_cb;
    void                    *args;
} AtHandleCb_t;

/**
 * @brief 配置at模块
 * @param handle_cb: 回调函数
 * @param catch_str: 需要被捕获的字符串
 * @param catch_str_cnt: 需要被捕获字符串的个数
 * @param fifo_len: 申请fifo的大小
 * @param at_cmd_num: 连续最大发送给框架的at指令数.
 *                    假设：框架一直处在接收当中，没有及时处理上层发送的at指令，
 *                          该参数就是限制上层能发送给框架的最大at指令数，
 *                          超过该数字，框架就会丢弃上层的at指令
 * @param is_cmd_ok: 指令AT+CIPSEND发送后，模块是否返回OK，有返回OK需要设置为1
 *
 * @note: 1, 在内存空间足够的情况下，at_cmd_num设置为0，表示可以无限开辟堆栈空间
 *        2，在堆栈空间有限的单片机中，需要设定at_cmd_num的大小，否则容易出现堆溢出，导致程序异常
 */
typedef struct {
    AtHandleCb_t    handle_cb;
    AtCatchStr_t    *catch_str;
    hy_u8_t         catch_str_cnt;
    hy_u32_t        fifo_len;
    hy_u8_t         at_cmd_num;
    hy_u8_t         is_cmd_ok;
} AtConfig_t;

/**
 * @brief 创建at处理框架
 *
 * @param handle_cb: 封装帧的回调函数和发送数据的回调函数
 * @param catch_str: 被检测的字符串
 * @param catch_str_cnt: 被检测的字符串的个数
 * @param fifo_len: fifo的长度
 *
 * @return 操作句柄
 */
void *HyAtUtilsCreate(AtConfig_t *config);

/**
 * @brief 销毁at处理框架
 *
 * @param handle: 操作句柄
 */
void HyAtUtilsDestroy(void *handle);

/**
 * @brief 清除at处理框架的信息，使其回到创建初始状态
 *
 * @param handle: 操作句柄
 */
void HyAtUtilsClean(void *handle);

/**
 * @brief 接管at模块返回的数据
 *
 * @param handle: 操作句柄
 * @param buf: at模块的数据
 * @param len: 数据的长度
 *
 * @return 返回成功写入的长度
 */
hy_u32_t HyAtUtilsPutData(void *handle, const char *buf, hy_u32_t len);

/**
 * @brief 写入at指令
 *
 * @param handle: 操作句柄
 * @param at_cmd: 需要发送的指令
 *
 * @return 成功写入返回ERR_OK
 */
hy_u32_t HyAtUtilsWriteCmd(void *handle, AtCmd_t *at_cmd);

/**
 * @brief 框架处理函数
 *
 * @param handle: 操作句柄
 *
 * @return 返回捕获字符串的对应状态
 */
hy_u8_t HyAtUtilsProcess(void *handle);

#ifdef __cplusplus
}
#endif

#endif

