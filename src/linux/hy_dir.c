/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_dir.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    09/10 2023 09:27
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        09/10 2023      create the file
 * 
 *     last modified: 09/10 2023 09:27
 */
#include <stdio.h>
#include <dirent.h>

#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_string.h"

#include "hy_dir.h"

static void _filter_file(const char *path, const char *name,
                         const char *filter, HyDirType_e type,
                         HyDirReadDirCb_t read_dir_cb, void *args)
{
    char buf[64] = {0};

    if (!read_dir_cb) {
        LOGD("the dir read cb is NULL \n");
        return;
    }

    if (filter) {
        if (HY_STRLEN(filter) < sizeof(buf)) {
            buf[0] = '.';
            HyStrCopyRight(name, buf + 1, sizeof(buf), '.');
            if (0 == HY_STRNCMP(buf, filter, strlen(filter))) {
                read_dir_cb(path, name, type, args);
            }
        } else {
            LOGE("the suffix is too long \n");
        }
    } else {
        read_dir_cb(path, name, type, args);
    }
}

static void _handle_sub_dir(const char *path, const char *name,
                            const char *filter, HyDirType_e type,
                            HyDirReadDirCb_t read_dir_cb, void *args)
{
    hy_u32_t len = 0;
    char *sub_path;

    len = HY_STRLEN(path) + HY_STRLEN(name) + 1 + 1; // 1 for space('\0'), 1 for '/'
    len = HY_MEM_ALIGN4_UP(len);
    sub_path = HY_MEM_CALLOC_RETURN(char *, len);
    if (!sub_path) {
        LOGE("malloc failed \n");
        return ;
    }

    if (path[HY_STRLEN(path) - 1] == '/') {
        snprintf(sub_path, len, "%s%s/", path, name);
    } else {
        snprintf(sub_path, len, "%s/%s/", path, name);
    }
    HyDirRead(sub_path, filter, HY_DIR_RECURSE_OPEN, read_dir_cb, args);

    HY_MEM_FREE_PP(&sub_path);
}

hy_s32_t HyDirRead(const char *path, const char *filter, hy_s32_t recurse_flag,
                   HyDirReadDirCb_t read_dir_cb, void *args)
{
    DIR *dir;
    struct dirent *ptr;

    HY_ASSERT_RET_VAL(!path || !read_dir_cb, -1);

    dir = opendir(path);
    if (!dir) {
        LOGES("opendir failed \n");
        return -1;
    }

    while ((ptr = readdir(dir)) != NULL) {
        if (HY_STRNCMP(ptr->d_name, ".", 1) == 0 || HY_STRNCMP(ptr->d_name, "..", 2) == 0) {
            continue;
        }

        switch (ptr->d_type) {
            case DT_REG:
                _filter_file(path, ptr->d_name, filter, HY_DIR_TYPE_FILE, read_dir_cb, args);
                break;

            case DT_DIR:
                if (recurse_flag == HY_DIR_RECURSE_OPEN) {
                    _handle_sub_dir(path, ptr->d_name, filter, HY_DIR_TYPE_DIR, read_dir_cb, args);
                } else {
                    _filter_file(path, ptr->d_name, filter, HY_DIR_TYPE_FILE, read_dir_cb, args);
                }
                break;

            default:
                LOGE("error type: %d \n", ptr->d_type);
                break;
        }
    }

    return 0;
}
