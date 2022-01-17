/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_socket.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    17/01 2022 08:32
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        17/01 2022      create the file
 * 
 *     last modified: 17/01 2022 08:32
 */
#include <stdio.h>

#include "hy_hal/hy_assert.h"
#include "hy_hal/hy_log.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_mem.h"

#include "hy_socket.h"
#include "hy_socket_inside.h"
#include "hy_socket_server.h"

void HySocketDestroy(void **handle)
{
}

void *HySocketCreate(HySocketConfig_s *config)
{
    LOGT("config: %p \n", config);
    HY_ASSERT_VAL_RET_VAL(!config, NULL);

    hy_socket_context_s *context = NULL;
    do {
        context = HY_MEM_MALLOC_BREAK(hy_socket_context_s *, sizeof(*context));

        HySocketSaveConfig_s *save_config = &config->save_config;
        HY_MEMCPY(&context->save_config, save_config, sizeof(*save_config));

        if (save_config->type == HY_SOCKET_TYPE_SERVER) {
        } else {
        }

        return context;
    } while (0);

    return NULL;
}
