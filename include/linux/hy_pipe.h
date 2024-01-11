/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_pipe.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    03/03 2022 19:04
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        03/03 2022      create the file
 * 
 *     last modified: 03/03 2022 19:04
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_PIPE_H_
#define __LIBHY_UTILS_INCLUDE_HY_PIPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hy_os_type/hy_os_type.h>

/**
 * @brief pipe阻塞状态
 */
typedef enum {
    HY_PIPE_BLOCK_STATE_BLOCK,              ///< 阻塞
    HY_PIPE_BLOCK_STATE_NOBLOCK,            ///< 非阻塞
} HyPipeBlockState_e;

/**
 * @brief 配置参数
 */
typedef struct {
} HyPipeSaveConfig_s;

/**
 * @brief 配置参数
 */
typedef struct {
    HyPipeSaveConfig_s  save_c;
    HyPipeBlockState_e  read_block_state;   ///< 读端阻塞状态
} HyPipeConfig_s;

typedef struct HyPipe_s HyPipe_s;

/**
 * @brief 创建pipe
 *
 * @param pipe_c 配置参数
 *
 * @return 成功返回句柄，失败返回NULL
 */
HyPipe_s *HyPipeCreate(HyPipeConfig_s *pipe_c);

/**
 * @brief 创建pipe宏
 *
 * @param _read_block_state 读端阻塞状态
 *
 * @return 成功返回句柄，失败返回NULL
 */
#define HyPipeCreate_m(_read_block_state)           \
({                                                  \
    HyPipeConfig_s pipe_c;                          \
    memset(&pipe_c, 0, sizeof(pipe_c));             \
    pipe_c.read_block_state = _read_block_state;    \
    HyPipeCreate(&pipe_c);                          \
})

/**
 * @brief 销毁pipe
 *
 * @param handle_pp 句柄的地址(二级指针)
 */
void HyPipeDestroy(HyPipe_s **handle_pp);

/**
 * @brief 向pipe中读取数据
 *
 * @param handle 句柄
 * @param buf 数据的地址
 * @param len 数据的长度
 *
 * @return
 *   成功返回读到的字节数，
 *   失败返回-1，
 *   被中断返回0
 */
hy_s32_t HyPipeRead(HyPipe_s *handle, void *buf, hy_s32_t len);

/**
 * @brief 向pipe中写入数据
 *
 * @param handle 句柄
 * @param buf 数据的地址
 * @param len 数据的长度
 *
 * @return 成功返回len，失败返回-1
 */
hy_s32_t HyPipeWrite(HyPipe_s *handle, const void *buf, hy_s32_t len);

/**
 * @brief 获取读端fd
 *
 * @param handle 句柄
 *
 * @return 成功返回fd，失败返回-1
 */
hy_s32_t HyPipeReadFdGet(HyPipe_s *handle);

/**
 * @brief 获取写端fd
 *
 * @param handle 句柄
 *
 * @return 成功返回fd，失败返回-1
 */
hy_s32_t HyPipeWriteFdGet(HyPipe_s *handle);

#ifdef __cplusplus
}
#endif

#endif
