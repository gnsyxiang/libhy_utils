/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_sort_test.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    30/10 2021 13:45
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        30/10 2021      create the file
 * 
 *     last modified: 30/10 2021 13:45
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hy_hal/hy_type.h"
#include "hy_hal/hy_mem.h"
#include "hy_hal/hy_string.h"
#include "hy_hal/hy_hal_utils.h"
#include "hy_hal/hy_log.h"

#include "hy_sort.h"

typedef struct {
    void *log_h;
} _main_context_t;

static hy_s32_t _swap_int_cb(void *src, void *dst)
{
    hy_s32_t  *a = src;
    hy_s32_t  *b = dst;

    return *a - *b;
}

static void _dump_int(hy_s32_t *a, hy_u32_t len)
{
    for (hy_u32_t i = 0; i < len; ++i) {
        printf("%d ", *(a + i));
    }
    printf("\n");
}

static void _test_int(void)
{
    hy_s32_t a[] = {3, 4, 1, 5, 8, 0, 9};
    hy_u32_t len = HyHalUtilsArrayCnt(a);

    _dump_int(a, len);
    // HySortBubble(a, len, sizeof(a[0]),  _swap_int_cb);
    HySortQuick(a, 0, len - 1, sizeof(a[0]),  _swap_int_cb);
    _dump_int(a, len);
}

typedef struct {
    hy_u32_t id;
    char        name[32];
    float       score[3];
} _student_t;

static hy_s32_t _swap_stu_cb(void *src, void *dst)
{
    _student_t *a = src;
    _student_t *b = dst;

    return (hy_s32_t)(a->score[0] - b->score[0]);
    // return a->id - b->id;
}

static void _dum_student(_student_t *stu, hy_u32_t len)
{
    for (hy_u32_t i = 0; i < len; ++i) {
        printf("id: %d, name: %s, score: %f, %f, %f \n",
                stu[i].id, stu[i].name, stu[i].score[0],stu[i].score[1], stu[i].score[2]);
    }
}

static void _test_struct(void)
{
    _student_t stu[] = {
        {1001, "tom", {77, 22, 33}},
        {1007, "jim", {44, 55, 99}},
        {1003, "jac", {11, 88, 66}},
    };
    hy_u32_t len = HyHalUtilsArrayCnt(stu);

    _dum_student(stu, len);
    // HySortBubble(stu, len, sizeof(stu[0]),  _swap_stu_cb);
    HySortQuick(stu, 0, len - 1, sizeof(stu[0]),  _swap_stu_cb);
    _dum_student(stu, len);
}

int main(int argc, char *argv[])
{
    _main_context_t *context = HY_MEM_MALLOC_RET_VAL(_main_context_t *,
            sizeof(*context), -1);

    context->log_h = HyLogCreate_m(512, 512,
            HY_LOG_LEVEL_TRACE, HY_TYPE_FLAG_ENABLE);
    if (!context->log_h) {
        LOGE("HyLogCreate_m failed \n");
        return -1;
    }

    LOGE("version: %s, data: %s, time: %s \n", "0.1.0", __DATE__, __TIME__);

    _test_int();
    _test_struct();

    HyLogDestroy(&context->log_h);

    HY_MEM_FREE_PP(&context);

    return 0;
}

