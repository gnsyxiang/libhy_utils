/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    format_cb.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    28/04 2022 10:21
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        28/04 2022      create the file
 * 
 *     last modified: 28/04 2022 10:21
 */
#include <pthread.h>
#include <stdio.h>
#include <inttypes.h>

#include "hy_printf.h"
#include "log_private.h"

#include "format_cb.h"

static hy_s32_t _format_log_color_cb(dynamic_array_s *dynamic_array,
                                     HyLogAddiInfo_s *addi_info)
{
    char buf[16] = {0};
    hy_s32_t ret = 0;
    hy_char_t *color[][2] = {
        {"F", PRINT_FONT_RED},
        {"E", PRINT_FONT_RED},
        {"W", PRINT_FONT_YEL},
        {"I", ""},
        {"D", PRINT_FONT_GRE},
        {"T", ""},
    };

    ret = snprintf(buf, sizeof(buf), "%s", color[addi_info->level][1]);

    return dynamic_array_write(dynamic_array, buf, ret);
}

static hy_s32_t _format_log_level_info_cb(dynamic_array_s *dynamic_array,
                                          HyLogAddiInfo_s *addi_info)
{
    char buf[4] = {0};
    hy_s32_t ret = 0;
    hy_char_t *color[][2] = {
        {"F", PRINT_FONT_RED},
        {"E", PRINT_FONT_RED},
        {"W", PRINT_FONT_YEL},
        {"I", ""},
        {"D", PRINT_FONT_GRE},
        {"T", ""},
    };

    ret = snprintf(buf, sizeof(buf), "[%s]", color[addi_info->level][0]);

    return dynamic_array_write(dynamic_array, buf, ret);
}

static hy_s32_t _format_log_time_cb(dynamic_array_s *dynamic_array,
                                    HyLogAddiInfo_s *addi_info)
{
    char buf[32] = {0};
    hy_u32_t ret = 0;

    ret = log_time(buf, sizeof(buf));
    return dynamic_array_write(dynamic_array, buf, ret);
}

static hy_s32_t _format_log_pid_id_cb(dynamic_array_s *dynamic_array,
                                      HyLogAddiInfo_s *addi_info)
{
    char buf[64] = {0};
    hy_s32_t ret = 0;
    char name[16] = {0};

    pthread_getname_np(addi_info->tid, name, sizeof(name));
    ret = snprintf(buf, sizeof(buf),
                   "[%ld-0x%lx(%s)]", addi_info->pid,
                   addi_info->tid, name);

    return dynamic_array_write(dynamic_array, buf, ret);
}

static hy_s32_t _format_log_func_line_cb(dynamic_array_s *dynamic_array,
                                         HyLogAddiInfo_s *addi_info)
{
    char buf[128] = {0};
    hy_s32_t ret = 0;

    if (addi_info->err_str) {
        ret += snprintf(buf + ret, sizeof(buf) - ret,
                       "[%s:%"PRId32"]", addi_info->func, addi_info->line);
        ret += snprintf(buf + ret, sizeof(buf) - ret,
                       "[errno: %d, err: %s] ", errno, addi_info->err_str);
    } else {
        ret += snprintf(buf + ret, sizeof(buf) - ret,
                       "[%s:%"PRId32"] ", addi_info->func, addi_info->line);
    }

    return dynamic_array_write(dynamic_array, buf, ret);
}

static hy_s32_t _format_log_usr_msg_cb(dynamic_array_s *dynamic_array,
                                       HyLogAddiInfo_s *addi_info)
{
    return dynamic_array_write_vprintf(dynamic_array,
                                       addi_info->fmt, addi_info->str_args);
}

static hy_s32_t _format_log_color_reset_cb(dynamic_array_s *dynamic_array,
                                           HyLogAddiInfo_s *addi_info)
{
    return dynamic_array_write(dynamic_array,
                               PRINT_ATTR_RESET, strlen(PRINT_ATTR_RESET));
}

void format_cb_register(format_cb_t **format_cb_pp, hy_u32_t *format_cb_cnt, hy_u32_t format)
{
    if (!format_cb_pp || *format_cb_cnt) {
        log_error("the param is NULL \n");
        return;
    }

    struct {
        HyLogOutputFormat_e     format;
        format_cb_t         format_log_cb;
    } log_format_cb[] = {
        {HY_LOG_OUTPUT_FORMAT_COLOR,        {_format_log_color_cb,          NULL,                       }},
        {HY_LOG_OUTPUT_FORMAT_LEVEL_INFO,   {_format_log_level_info_cb,     _format_log_level_info_cb,  }},
        {HY_LOG_OUTPUT_FORMAT_TIME,         {_format_log_time_cb,           _format_log_time_cb,        }},
        {HY_LOG_OUTPUT_FORMAT_PID_ID,       {_format_log_pid_id_cb,         _format_log_pid_id_cb,      }},
        {HY_LOG_OUTPUT_FORMAT_FUNC_LINE,    {_format_log_func_line_cb,      _format_log_func_line_cb,   }},
        {HY_LOG_OUTPUT_FORMAT_USR_MSG,      {_format_log_usr_msg_cb,        _format_log_usr_msg_cb,     }},
        {HY_LOG_OUTPUT_FORMAT_COLOR_RESET,  {_format_log_color_reset_cb,    NULL,                       }},
    };
    hy_u32_t cnt = LOG_ARRAY_CNT(log_format_cb);
    format_cb_t *format_cb = calloc(cnt, sizeof(format_cb_t));
    if (!format_cb) {
        log_error("calloc failed \n");
        return;
    }

    for (hy_u32_t i = 0; i < cnt; ++i) {
        if (log_format_cb[i].format == (format & 0x1 << i)) {
            memcpy(format_cb[i], log_format_cb[i].format_log_cb,
                   sizeof(log_format_cb[i].format_log_cb));
        }
    }

    *format_cb_pp = format_cb;
    *format_cb_cnt = cnt;
}

