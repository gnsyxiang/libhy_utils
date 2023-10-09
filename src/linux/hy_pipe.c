/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_pipe.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    04/03 2022 08:24
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        04/03 2022      create the file
 * 
 *     last modified: 04/03 2022 08:24
 */
#include <stdio.h>
#include <unistd.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_string.h"
#include "hy_file.h"

#include "hy_pipe.h"

struct HyPipe_s {
    hy_s32_t pfd[2];
};

hy_s32_t HyPipeReadFdGet(HyPipe_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->pfd[0];
}

hy_s32_t HyPipeWriteFdGet(HyPipe_s *handle)
{
    HY_ASSERT_RET_VAL(!handle, -1);

    return handle->pfd[1];
}

hy_s32_t HyPipeRead(HyPipe_s *handle, void *buf, hy_s32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    return HyFileRead(handle->pfd[0], buf, len);

}

hy_s32_t HyPipeWrite(HyPipe_s *handle, const void *buf, hy_s32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    return HyFileWriteN(handle->pfd[1], buf, len);
}

void HyPipeDestroy(HyPipe_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);

    HyPipe_s *handle = *handle_pp;

    close(handle->pfd[1]);
    close(handle->pfd[0]);

    LOGI("hy pipe destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyPipe_s *HyPipeCreate(HyPipeConfig_s *pipe_c)
{
    HY_ASSERT_RET_VAL(!pipe_c, NULL);

    HyPipe_s *context = NULL;
    HyFileBlockState_e state = HY_FILE_BLOCK_STATE_BLOCK;

    do {
        context = HY_MEM_MALLOC_BREAK(HyPipe_s *, sizeof(*context));

        if (0 != pipe(context->pfd)) {
            LOGE("pipe failed \n");
            break;
        }

        if (HY_PIPE_BLOCK_STATE_NOBLOCK == pipe_c->read_block_state) {
            state = HY_FILE_BLOCK_STATE_NOBLOCK;
        }
        if (0 != HyFileBlockStateSet(context->pfd[0], state)) {
            LOGE("hy file block state set failed \n");
            break;
        }

        LOGI("hy pipe create, handle: %p \n", context);
        return context;
    } while (0);

    LOGE("hy pipe create failed \n");
    HyPipeDestroy(&context);
    return NULL;
}
