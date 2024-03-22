/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_at_utils.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hy_os_type/hy_os_type.h>

#include "hy_at_utils.h"

#include "hy_time.h"
#include "hy_socket.h"
#include "hy_fifo.h"
#include "hy_log.h"

#ifdef USE_DEBUG
    #define ALONE_DEBUG 1
    #define LOG_CATEGORY_TAG "at_utils"
    // #define SAVE_TEST_DATA
#endif

typedef struct {
    struct list_head    write_list;
    void                *fifo_handle;

    hy_u32_t            sys_tick;

    BufUnion_t          *buf_union;
    hy_u32_t            frame_sum;
    hy_u16_t            offset_len;

    hy_s8_t              state;
    hy_s8_t              busy_flag;
    hy_u8_t             parsing_flag;

#ifdef SAVE_TEST_DATA
    hy_u32_t            in_len;
    hy_u32_t            out_len;
    char                *in_char;
    char                *out_char;
#endif

    hy_u8_t             is_cmd_ok;
    hy_u8_t             at_cmd_node_cnt;
    hy_u8_t             at_cmd_num;
    AtHandleCb_t        handle_cb;
    hy_u8_t             catch_str_cnt;
    AtCatchStr_t        catch_str[0];
} at_utils_context_t;

#define _get_fifo_used_len() HyFifoGetInfo(context->fifo_handle, HY_FIFO_USED_LEN)

#if 1
typedef struct {
    struct list_head    list;
    AtCmd_t             at_cmd;
    hy_u8_t             del_cnt;
} at_cmd_list_t;

static at_cmd_list_t *_at_cmd_list_create(at_utils_context_t *context, AtCmd_t *at_cmd)
{
    at_cmd_list_t *at_cmd_list = calloc(1, sizeof(*at_cmd_list));
    if (!at_cmd_list) {
        LOGE("calloc faild \n");
        return NULL;
    }

    hy_u32_t len = strlen(at_cmd->cmd);
    at_cmd_list->at_cmd.cmd = calloc(1, len + 1);
    if (!at_cmd_list->at_cmd.cmd) {
        LOGE("calloc faild \n");
        free(at_cmd_list);
        at_cmd_list = NULL;
        return NULL;
    }

    strncpy(at_cmd_list->at_cmd.cmd, at_cmd->cmd, len);
    at_cmd_list->del_cnt++;

    if (at_cmd->data_len > 0) {
        at_cmd_list->at_cmd.data = calloc(1, at_cmd->data_len);
        if (!at_cmd_list->at_cmd.data) {
            LOGE("calloc faild \n");
            return NULL;
        }
        memcpy(at_cmd_list->at_cmd.data, at_cmd->data, at_cmd->data_len);
        at_cmd_list->at_cmd.data_len = at_cmd->data_len;
        if (context->is_cmd_ok) {
            at_cmd_list->del_cnt++;
        }
    }

    at_cmd_list->at_cmd.retry         = at_cmd->retry;
    at_cmd_list->at_cmd.wait_time     = at_cmd->wait_time;

    return at_cmd_list;
}

static void _at_cmd_list_destroy(at_cmd_list_t *at_cmd_list)
{
    if (at_cmd_list) {
        if (at_cmd_list->at_cmd.cmd) {
            free(at_cmd_list->at_cmd.cmd);
            at_cmd_list->at_cmd.cmd = NULL;
        }

        if (at_cmd_list->at_cmd.data) {
            free(at_cmd_list->at_cmd.data);
            at_cmd_list->at_cmd.data = NULL;
        }

        free(at_cmd_list);
        at_cmd_list = NULL;
    }
}
#endif

static void _destroy_cmd_list(at_utils_context_t *context, hy_s8_t all_cmd)
{
    at_cmd_list_t *pos, *n;
    list_for_each_entry_safe(pos, n, &context->write_list, list) {
        LOGD("pos: %p, del at_cmd: %s", pos, pos->at_cmd);

        context->at_cmd_node_cnt--;
        list_del(&pos->list);
        _at_cmd_list_destroy(pos);
        pos = NULL;
        if (!all_cmd) {
            break;
        }
    }
}

static void _destroy_clean(at_utils_context_t *context)
{
    _destroy_cmd_list(context, 1);
    HyFifoClean(context->fifo_handle);

    BufUnion_t **ppbuf_union = &context->buf_union;
    *ppbuf_union             = NULL;
    context->sys_tick        = 0;
    context->frame_sum       = 0;
    context->offset_len      = 0;
    context->state           = 0;
    context->busy_flag       = 0;
    context->parsing_flag    = 0;
}

static hy_u8_t _find_frame_tag(at_utils_context_t   *context,
                               char                 *buf,
                               hy_u16_t             buf_len,
                               hy_u8_t              flag)
{
    AtCatchStr_t *catch_str = NULL;
    hy_u8_t ret = AT_STATE_IDLE;

    if (flag) {
        for (hy_s32_t i = 0; i < context->catch_str_cnt; i++) {
            catch_str = &context->catch_str[i];
            hy_u16_t len = strlen(catch_str->catch_str);
            // 从fifo获取的数据长度不足，等待有效数据（比如: buf为“+IPD”，则直接跳回）
            if (0 == memcmp(catch_str->catch_str, buf, 2) && len >buf_len) {
                ret = AT_STATE_READ_HEAD;
                goto _ERR_FIND_FRAME_TAG;
            }
            if (0 == memcmp(buf, catch_str->catch_str, len)) {
                context->offset_len = len;
                ret         = catch_str->state;

                LOGD("find '%s' \n", catch_str->catch_str);
                goto _ERR_FIND_FRAME_TAG;
            }
        }
        context->offset_len = 1;
    } else {
        hy_u32_t cnt = 0;
        while (cnt < buf_len) {
            for (hy_s32_t i = 0; i < context->catch_str_cnt; i++) {
                catch_str = &context->catch_str[i];
                if (catch_str->catch_str[0] == buf[cnt]) {
                    context->offset_len = cnt;
                    ret         = AT_STATE_READ_HEAD;

                    LOGI("find '%c' \n", buf[cnt]);
                    goto _ERR_FIND_FRAME_TAG;
                }
            }
            cnt++;
        }
        context->offset_len = cnt;
    }
_ERR_FIND_FRAME_TAG:
    return ret;
}


static hy_s32_t _get_signal_str(char *buf)
{
    hy_u32_t flag = 0;
    hy_u32_t ret = 0;

    while (*buf) {
        if (*buf == 'O' && buf[1] == 'K') {
            flag = 1;
            // ret += 2; // 把OK放到后面处理，用于从发送链表中删除命令
            break;
        }
        ret++;
        buf++;
    }

    if (!flag) {
        return 0;
    } else {
        return ret;
    }
}

static hy_s8_t _handle_signal_csq(at_utils_context_t *context, char *buf, hy_u16_t len)
{
#define SIGNAL_STR_MIN "+CSQ: 0, 0\r\n\r\nOK"
    if (len < strlen(SIGNAL_STR_MIN)) {
        return ERR_FAILD;
    }

    hy_u32_t ret = _get_signal_str(buf);
    if (0 == ret) {
        return ERR_FAILD;
    }

    context->buf_union = HyBufUnionCreate(ret);
    if (!context->buf_union) {
        LOGE("buf_union create faild \n");
        return ERR_FAILD;
    }

    HY_UTILS_COPY(context->buf_union->buf, buf, context->buf_union->len);
    return ERR_OK;
}

static hy_s8_t _handle_signal_cwjap_def(at_utils_context_t *context, char *buf, hy_u16_t len)
{
    #define SIGNAL_DATA_LEN_MIX "+CWJAP_DEF:,\"50:fa:84:2e:4b:0c\",12,-62"
    if (len < strlen(SIGNAL_DATA_LEN_MIX)) {
        return ERR_FAILD;
    }

    hy_u32_t ret = _get_signal_str(buf);
    if (0 == ret) {
        return ERR_FAILD;
    }

    context->buf_union = HyBufUnionCreate(ret);
    if (!context->buf_union) {
        LOGE("buf_union create faild \n");
        return ERR_FAILD;
    }

    HY_UTILS_COPY(context->buf_union->buf, buf, context->buf_union->len);
    return ERR_OK;
}

static inline hy_u16_t _get_ipd_data_len(char *buf, hy_u8_t len, hy_u16_t *index)
{
#define SKIP_IPD_DATA           "+IPD,:"
#define SSCANF_BUF_LEN             (8)
    hy_u16_t ret = 0;
    if (len >= sizeof("+IPD,1:")) {
        char sscanf_buf[SSCANF_BUF_LEN] = {0};
        sscanf(buf, "%*[^,],%[^:]", sscanf_buf);
        hy_s32_t i = 0;
        for (i = 0; i < SSCANF_BUF_LEN; i++) {
            if (0 != sscanf_buf[i] && (sscanf_buf[i] < '0' || sscanf_buf[i] > '9')) {
                *index = 0;
                break;
            }
        }
        if (i >= SSCANF_BUF_LEN) {
            *index = atoi(sscanf_buf);
        }
        ret = strlen(SKIP_IPD_DATA) + strlen(sscanf_buf);
    }
    return ret;
}

static hy_s8_t _handle_ipd_data(at_utils_context_t *context, char *buf, hy_u16_t len)
{
#define _WAIT_READ_FIFO_DATA()                                      \
    do {                                                            \
        hy_u8_t read_cnt = 0;                                       \
        while (_get_fifo_used_len() < factor) {                     \
            HyTimeDelayMs(5);                                       \
            if (read_cnt++ >= 5) {                                  \
                LOGD("wait uart data \n");                        \
                break;                                              \
            }                                                       \
        }                                                           \
        context->parsing_flag = 1;                                  \
    } while (0)
#define FRAME_IPD_LEN_MAX       strlen("+IPD,5120:")

    hy_u8_t ipd_format_len = 0;
    hy_u16_t data_len = 0;
    hy_s8_t ret = ERR_FAILD;
    hy_u16_t factor = 0;

    if (!context->buf_union) {
        ipd_format_len = _get_ipd_data_len(buf, len, &data_len);
        LOGD("ipd_format_len: %d, socket len: %d \n", ipd_format_len, data_len);

        // 防御性判断，因为fifo开辟的空间有限，如果长时间输入数据就会造成错误帧的出现
        if (ipd_format_len >= FRAME_IPD_LEN_MAX
                || data_len > 1024 || 0 == data_len) {
            // @FIXME 后期处理
            LOGE("reset state, write fifo and read fifo\n");
            _destroy_clean(context);
            return ERR_FAILD;
        }

        context->buf_union = HyBufUnionCreate(data_len);
        if (!context->buf_union) {
            LOGE("buf_union create faild \n");
            return ERR_FAILD;
        }

        if (context->buf_union->len + ipd_format_len <= len) {
            memcpy(context->buf_union->buf, buf + ipd_format_len, context->buf_union->len);
            context->offset_len = ipd_format_len + context->buf_union->len;
            ret = ERR_OK;
        } else {
            memcpy(context->buf_union->buf, buf + ipd_format_len, len - ipd_format_len);
            context->frame_sum += (len - ipd_format_len);
            context->offset_len = len;
            factor = context->buf_union->len - (len - ipd_format_len);
            _WAIT_READ_FIFO_DATA();
        }
    } else {
        memcpy(context->buf_union->buf + context->frame_sum, buf, len);
        context->frame_sum += len;
        context->offset_len = len;
        if (context->frame_sum == context->buf_union->len) {
            ret = ERR_OK;
        } else {
            factor = context->buf_union->len - context->frame_sum; 
            _WAIT_READ_FIFO_DATA();
        }
    }

    return ret;
}

static hy_u8_t _read_handle(at_utils_context_t *context)
{
#define _HANDLE_FRAME_CB(context, flag)                                             \
    do {                                                                            \
        AtHandleCb_t *handle_cb = &context->handle_cb;                              \
        BufUnion_t **ppbuf = &context->buf_union;                                   \
        BufUnion_t *buf = *ppbuf;                                                   \
        if (handle_cb->handle_frame_cb) {                                           \
            handle_cb->handle_frame_cb(flag, &buf->buf, buf->len, handle_cb->args); \
        }                                                                           \
        HyBufUnionDestroy(buf);                                                     \
        *ppbuf = NULL;                                                              \
    } while (0)
#define FACTOR_LEN (200)

    hy_u16_t len = 0;
    hy_u16_t factor = 0;
    hy_u8_t ret = 0;
    at_cmd_list_t *pos;
    hy_s8_t update_flag = 0;

    do {
        char buf[FACTOR_LEN] = {0};
        memset(buf, '\0', FACTOR_LEN);
        if (!context->buf_union) {
            factor = FACTOR_LEN;
        } else {
            factor = context->buf_union->len - context->frame_sum;
            if (factor > FACTOR_LEN) {
                factor = FACTOR_LEN;
            }
        }

        pos = list_first_entry(&context->write_list, at_cmd_list_t, list);

        len = HyFifoPeek(context->fifo_handle, buf, factor);
        // PRINT_HEX_ASCII(buf, len);
        if (0 == len) {
            context->state = AT_STATE_IDLE;
            if (context->buf_union) {
                PRINT_HEX_ASCII(context->buf_union->buf, context->frame_sum);
                if (!pos) {
                    LOGD("pos time: %d \n", pos->at_cmd.wait_time);
                }
            }
            LOGD("peek fifo zero, factor: %d \n", factor);
            break;
        }

        // 接着处理被打断的接收任务（esp8266里面有缓冲，
        // 可能上一帧的数据在在后面传过来，而此时state又重新开始计算，导致上一帧出现问题）
        if (context->parsing_flag) {
            context->state = AT_STATE_READ_IPD_DATA;
        }

        switch (context->state) {
            case AT_STATE_IDLE:
            case AT_STATE_READ:
                context->state = _find_frame_tag(context, buf, len, 0);
                if (context->offset_len > 0) {
                    update_flag = 1;
                }
                break;
            case AT_STATE_READ_HEAD:
                context->state = _find_frame_tag(context, buf, len, 1);
                if (AT_STATE_IDLE == context->state && context->offset_len) {
                    if (_get_fifo_used_len() == 1 && buf[0] == '>') {
                        context->state = AT_STATE_READ_SEND;
                        LOGE("only one byte, '>' \n");
                        break;
                    }
                    update_flag = 1;
                }
                break;
            case AT_STATE_READ_READY:
                ret             = context->state;
                update_flag     = 1;
                context->state  = AT_STATE_IDLE;
                break;
            case AT_STATE_READ_BUSY_S:
                update_flag     = 1;
                context->state  = AT_STATE_READ;
                break;
            case AT_STATE_READ_CLOSED:
                LOGE("closed or error \n");
                context->parsing_flag   = 0;         // 一定要退出循环
                ret                     = AT_STATE_READ_CLOSED;
                update_flag             = 1;
                break;
            case AT_STATE_READ_ERROR:
            case AT_STATE_READ_FAILD:
                HyFifoClean(context->fifo_handle);
                ret = context->state;
                if (--pos->at_cmd.retry == 0) {
                    LOGE("faild exec %s", pos->at_cmd.cmd);
                    ret = AT_STATE_READ_CLOSED;
                    _destroy_cmd_list(context, 0);
                }
                context->state = AT_STATE_WRITE;
                break;
            case AT_STATE_READ_CONNECT_OK:
            case AT_STATE_READ_OK:
                if (2 == context->busy_flag || 0 == pos->at_cmd.data_len) {
                    context->busy_flag  = 0;
                }
                if (--pos->del_cnt == 0) {
                    _destroy_cmd_list(context, 0);
                }
                update_flag         = 1;
                ret                 = context->state;
                context->state      = AT_STATE_IDLE;
                // LOGD("AT_STATE_READ_OK or AT_STATE_READ_CONNECT_OK \n");
                break;
            case AT_STATE_READ_SEND:
                update_flag     = 1;
                context->state  = AT_STATE_WRITE_DATA;
                break;
            case AT_STATE_READ_IPD_DATA:
                if (ERR_OK == _handle_ipd_data(context, buf, len)) {
                    context->frame_sum = 0;
                    context->parsing_flag   = 0;
                    context->state          = AT_STATE_IDLE;
                    _HANDLE_FRAME_CB(context, 1);
                }
                if (context->offset_len > 0) {
                    update_flag = 1;
                }
                break;
            case AT_STATE_READ_SIGNAL_CSQ:
                if (ERR_OK == _handle_signal_csq(context, buf, len)) {
                    update_flag = 1;
                    context->offset_len = context->buf_union->len;
                    context->state = AT_STATE_IDLE;
                    _HANDLE_FRAME_CB(context, 0);
                }
                break;
            case AT_STATE_READ_SIGNAL_CWJAP_DEF:
                if (ERR_OK == _handle_signal_cwjap_def(context, buf, len)) {
                    update_flag = 1;
                    context->offset_len = context->buf_union->len;
                    context->state = AT_STATE_IDLE;
                    _HANDLE_FRAME_CB(context, 0);
                }
                break;
            default:
                break;
        }

        if (update_flag) {
            update_flag = 0;
#ifdef SAVE_TEST_DATA
            memcpy(context->out_char + context->out_len, buf, context->offset_len);
            context->out_len += context->offset_len;
#endif
            // PRINT_HEX_ASCII(buf, context->offset_len);
            HyFifoUpdateOut(context->fifo_handle, context->offset_len);
            context->offset_len = 0;
        }

        // 接着处理被打断的接收任务（esp8266里面有缓冲，
        // 可能上一帧的数据在在后面传过来，而此时state又重新开始计算，导致上一帧出现问题）
    } while (context->parsing_flag);

    return ret;
}

static hy_u8_t _write_handle(at_utils_context_t *context)
{
    at_cmd_list_t *pos;
    AtHandleCb_t *handle_cb = NULL;

    // LOGD("state: %d \n", context->state);
    switch (context->state) {
        case AT_STATE_WRITE:
            context->state = AT_STATE_WRITE_CMD;
            break;
        case AT_STATE_WRITE_CMD:
            pos = list_first_entry(&context->write_list, at_cmd_list_t, list);
            handle_cb = &context->handle_cb;
            if (handle_cb->write_cb) {
                LOGD("pos: %p, %d times, wait_time: %d, cmd: %s",
                        pos, pos->at_cmd.retry, pos->at_cmd.wait_time, pos->at_cmd.cmd);

                handle_cb->write_cb(pos->at_cmd.cmd, strlen(pos->at_cmd.cmd), handle_cb->args);
                context->sys_tick   = HyTimeGetTickMs();
            }
            context->state      = AT_STATE_WRITE_CMD_WAIT;
            context->busy_flag  = 1;
            break;
        case AT_STATE_WRITE_CMD_WAIT:
            if (HyTimeGetTickMs() - context->sys_tick > 10) {
                // note: 这里不更新sys_tick，保证上层确定的总延时
                context->state      = AT_STATE_READ;
            }
            break;
        case AT_STATE_WRITE_DATA:
            pos = list_first_entry(&context->write_list, at_cmd_list_t, list);
            handle_cb = &context->handle_cb;
            if (pos && handle_cb->write_cb) {
                LOGD("pos: %p, cmd->data: %d \n", pos, pos->at_cmd.data_len);

                handle_cb->write_cb(pos->at_cmd.data, pos->at_cmd.data_len, handle_cb->args);
                context->sys_tick   = HyTimeGetTickMs();
                context->busy_flag  = 2;
            }
            context->state      = AT_STATE_READ;
            break;
    }
    return 0;
}

hy_u8_t HyAtUtilsProcess(void *handle)
{
    if (!handle) {
        LOGE("the param is NULL, handle: %p \n", handle);
        return 0;
    }

    at_utils_context_t *context = handle;
    hy_u8_t ret = 0;

    // LOGD("state: %d \n", context->state);
    switch (context->state) {
        case AT_STATE_IDLE:
            if (_get_fifo_used_len() > 0) {
                context->state = AT_STATE_READ;
            } else {
                // 链表不为空，需要发送数据
                if (!list_empty(&context->write_list)) {
                    // 规定的时间内没有返回，重新发送直到重启设备
                    at_cmd_list_t *pos = list_first_entry(&context->write_list, at_cmd_list_t, list);
                    if (pos) {
                        if (HyTimeGetTickMs() - context->sys_tick > pos->at_cmd.wait_time) {
                            context->sys_tick   = HyTimeGetTickMs();
                            context->busy_flag = 0;
                            if (0 == --pos->at_cmd.retry) {
                                LOGE("pos: %p, faild exec %s", pos, pos->at_cmd);
                                ret = AT_STATE_READ_CLOSED;
                                break;
                            }
                        }
                    }
                    if (0 == context->busy_flag) {
                        context->state = AT_STATE_WRITE;
                    }
                }
            }
            break;
        case AT_STATE_READ...AT_STATE_READ_MAX:
            ret = _read_handle(context);
            break;
        case AT_STATE_WRITE...AT_STATE_WRITE_MAX:
            ret = _write_handle(context);
            break;
        default:
            LOGE("error case, state: %d \n", context->state);
            break;
    }
    return ret;
}

hy_u32_t HyAtUtilsWriteCmd(void *handle, AtCmd_t *at_cmd)
{
    if (!handle || !at_cmd) {
        LOGE("the param is NULL, handle: %p, at_cmd: %p, retry: %d \n", handle, at_cmd);
        return ERR_FAILD;
    }
    at_utils_context_t *context = handle;

    if (0 != context->at_cmd_num && context->at_cmd_node_cnt >= context->at_cmd_num) {
        LOGE("write command too fast, cmd: %s \n", at_cmd->cmd);
        return ERR_FAILD;
    }

    at_cmd_list_t *at_cmd_list = _at_cmd_list_create(context, at_cmd);
    if (!at_cmd_list) {
        LOGE("at_cmd_list create faild \n");
        return ERR_FAILD;
    }

    list_add_tail(&at_cmd_list->list, &context->write_list);
    context->at_cmd_node_cnt++;

    LOGD("pos: %p, add at_cmd: %s", at_cmd_list, at_cmd_list->at_cmd.cmd);
    return ERR_OK;
}

hy_u32_t HyAtUtilsPutData(void *handle, const char *buf, hy_u32_t len)
{
    if (!handle || !buf) {
        LOGE("the param is NULL, handle: %p, buf_union: %p \n", handle, buf);
        return 0;
    }

    at_utils_context_t *context = handle;

#ifdef SAVE_TEST_DATA
    memcpy(context->in_char + context->in_len, buf, len);
    context->in_len += len;
#endif

    return HyFifoPut(context->fifo_handle, buf, len);
}

void HyAtUtilsClean(void *handle)
{
    if (!handle) {
        LOGE("the param is NULL \n");
        return ;
    }

    _destroy_clean((at_utils_context_t *)handle);
}

void HyAtUtilsDestroy(void *handle)
{
    if (!handle) {
        LOGE("the param is NULL \n");
        return ;
    }

    at_utils_context_t *context = handle;

    if (context) {
        if (context->fifo_handle) {
            HyFifoDestroy(context->fifo_handle);
            context->fifo_handle = NULL;
        }

        _destroy_cmd_list(context, 1);

        free(context);
        context = NULL;
    }
}

#ifdef SAVE_TEST_DATA
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "hy_signal.h"

static void _signal_cb(void *args)
{
    at_utils_context_t *context = args;

    hy_s32_t in_fd = open("./in.hex", O_RDWR|O_CREAT|O_TRUNC, 0777);
    if (in_fd < 0) {
        LOGE("open faild \n");
        return ;
    }
    hy_s32_t out_fd = open("./out.hex", O_RDWR|O_CREAT|O_TRUNC, 0777);
    if (out_fd < 0) {
        LOGE("open faild \n");
        return ;
    }

    write(in_fd, context->in_char, context->in_len);
    write(out_fd, context->out_char, context->out_len);

    free(context->in_char);
    free(context->out_char);

    close(in_fd);
    close(out_fd);
}
#endif

void *HyAtUtilsCreate(AtConfig_t *config)
{
    if (!config) {
        LOGE("the param is NULL \n");
        return NULL;
    }

    hy_u32_t len = config->catch_str_cnt * sizeof(*config->catch_str);
    at_utils_context_t *context = calloc(1, sizeof(*context) + len);
    if (!context) {
        LOGE("calloc faild \n");
        return NULL;
    }

    context->fifo_handle = HyFifoCreate(config->fifo_len);
    if (!context->fifo_handle) {
        LOGE("fifo create faild \n");
        free(context);
        context = NULL;
        return NULL;
    }

    INIT_LIST_HEAD(&context->write_list);
    memcpy(&context->handle_cb, &config->handle_cb, sizeof(config->handle_cb));

    context->is_cmd_ok      = config->is_cmd_ok;
    context->at_cmd_num     = config->at_cmd_num;
    context->catch_str_cnt  = config->catch_str_cnt;
    for (hy_s32_t i = 0; i < config->catch_str_cnt; i++) {
        memcpy(&context->catch_str[i], &config->catch_str[i], sizeof(*config->catch_str));
    }

#ifdef SAVE_TEST_DATA
    if (config->fifo_len == 1024) {
        HySignalHandleCb_t signal_handle_cb;
        signal_handle_cb.handle_frame_cb = _signal_cb;
        signal_handle_cb.args            = context;
        HySignalInit("./install/bin/alarm_speaker", "./", &signal_handle_cb);

        context->in_char = calloc(10 * 1024 * 1024, sizeof(*context->in_char));
        if (!context->in_char) {
            LOGE("calloc faild \n");
            return NULL;
        }

        context->out_char = calloc(10 * 1024 * 1024, sizeof(*context->out_char));
        if (!context->out_char) {
            LOGE("calloc faild \n");
            return NULL;
        }
    }
#endif

    return context;
}

