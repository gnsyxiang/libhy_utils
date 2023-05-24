/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_file.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    04/11 2021 08:08
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        04/11 2021      create the file
 * 
 *     last modified: 04/11 2021 08:08
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
/* According to POSIX.1-2001, POSIX.1-2008 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <hy_log/hy_log.h>

#include "hy_assert.h"
#include "hy_mem.h"
#include "hy_string.h"

#include "hy_file.h"

#if 0
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);

成功返回文件描述符，失败返回-1

O_RDONLY　　只读打开
O_WRONLY　  只写打开
O_RDWR      读写打开

O_APPEND    将写入追加到文件的尾端
O_CREAT     若文件不存在，则创建它。使用该选项时，需要第三个参数mode，用来指定新文件的访问权限位
O_EXCL      如果同时指定了O_CREAT，而文件已经存在，则会出错
O_TRUNC     如果此文件存在，而且为只写或读写模式成功打开，则将其长度截短为0
O_NOCTTY    如果pathname指的是终端设备，则不将该设备分配作为此进程的控制终端
O_NONBLOCK  如果pathname指的是一个FIFO文件、块设备文件或字符设备文件，则此选项将文件的本次打开操作和后续的I/O操作设置为非阻塞模式
#endif

hy_s32_t HyFileIsExist(const char *file_path)
{
    HY_ASSERT_RET_VAL(!file_path, -1);

    return access(file_path, F_OK);
}

void HyFileRemove(const char *file_path)
{
    HY_ASSERT_RET(!file_path);

    remove(file_path);
}

hy_s64_t HyFileGetLen(const char *file)
{
    HY_ASSERT(file);

    FILE *fp = NULL;
    hy_s64_t len = 0;

    do {
        fp = fopen(file, "r");
        if (!fp) {
            LOGES("fopen %s failed, fp: %p \n", file, fp);
            break;
        }

        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        if (len == -1) {
            LOGES("ftell failed, len: -1 \n");
            break;
        }

        fclose(fp);

        return len;
    } while (0);

    if (fp) {
        fclose(fp);
    }

    return -1;
}

hy_s32_t HyFileGetContent(const char *file, char **content)
{
    HY_ASSERT(file);
    HY_ASSERT(content);

    FILE *fp = NULL;
    char *buf = NULL;
    long len = 0;

    do {
        fp = fopen(file, "r");
        if (!fp) {
            LOGES("fopen %s failed, fp: %p \n", file, fp);
            break;
        }

        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        if (len == -1) {
            LOGES("ftell failed, len: -1 \n");
            break;
        }

        buf = malloc(len);
        if (!buf) {
            LOGES("malloc failed, buf: %p \n", buf);
            break;
        }

        fseek(fp, 0, SEEK_SET);
        len = fread(buf, 1, len, fp);

        *content = buf;
        fclose(fp);

        return len;
    } while (0);

    if (fp) {
        fclose(fp);
    }

    if (buf) {
        free(buf);
    }

    return -1;
}

hy_s32_t HyFileGetContent2(const char *file, char *content, hy_u32_t content_len)
{
    HY_ASSERT(file);

    FILE *fp = NULL;
    long len = content_len;

    fp = fopen(file, "r");
    if (!fp) {
        LOGES("fopen %s failed, fp: %p \n", file, fp);
        return -1;
    }

    len = fread(content, 1, len, fp);

    fclose(fp);

    return len;
}

hy_s32_t HyFileRead(hy_s32_t fd, void *buf, hy_u32_t len)
{
    hy_s32_t ret = 0;

_FILE_READ_AGAIN:
    ret = read(fd, buf, len);
    if (ret < 0) {
        if (EINTR == errno || EAGAIN == errno || EWOULDBLOCK == errno) {
            LOGW("read failed, errno: %d(%s) \n", errno, strerror(errno));
            ret = 0;
            goto _FILE_READ_AGAIN;
        } else {
            ret = -1;
            LOGES("read failed \n");
        }
    } else if (ret == 0) {
        LOGES("opposite fd close, fd: %d \n", fd);
        ret = -1;
    }

    return ret;
}

hy_s32_t HyFileReadN(hy_s32_t fd, void *buf, hy_u32_t len)
{
    hy_s32_t ret;
    hy_u32_t nleft;
    hy_u32_t offset = 0;

    nleft = len;

    while (nleft > 0) {
        ret = read(fd, buf + offset, nleft);
        // printf("file_wrapper->read, len: %d \n", ret);
        if (ret < 0) {
            if (EINTR == errno || EAGAIN == errno || EWOULDBLOCK == errno) {
                ret = 0;
            } else {
                LOGES("read failed \n");
                return -1;
            }
        } else if (ret == 0) {
            LOGES("opposite fd close, fd: %d \n", fd);
            break;
        } else {
            nleft  -= ret;
            offset += ret;
        }
    }

    return offset;
}

hy_s32_t HyFileReadTimeout(hy_s32_t fd, void *buf, hy_u32_t len, hy_u32_t ms)
{
    hy_u32_t ret;
    fd_set rfds;
    struct timeval time;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    time.tv_sec = ms / 1000;
    time.tv_usec = (ms % 1000) * 1000;

    ret = select(fd + 1, &rfds, NULL, NULL, &time);
    switch (ret) {
        case -1:
            LOGES("select error \n");
            break;
        case 0:
            LOGT("select timeout \n");
            break;
        default:
            ret = HyFileRead(fd, buf, len);
            break;
    }

    return ret;
}

hy_s32_t HyFileReadNTimeout(hy_s32_t fd, void *buf, hy_u32_t len, hy_u32_t ms)
{
    hy_s32_t ret;
    fd_set rfds;
    struct timeval time;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    time.tv_sec = ms / 1000;
    time.tv_usec = (ms % 1000) * 1000;

    ret = select(fd + 1, &rfds, NULL, NULL, &time);
    switch (ret) {
        case -1:
            LOGES("select error \n");
            break;
        case 0:
            LOGT("select timeout \n");
            break;
        default:
            ret = HyFileReadN(fd, buf, len);
            break;
    }

    return ret;
}

hy_s32_t HyFileWrite(hy_s32_t fd, const void *buf, hy_u32_t len)
{
    hy_s32_t ret;

_FILE_WRITE_AGAIN:
    ret = write(fd, buf, len);
    if (ret < 0 && (EINTR == errno || EAGAIN == errno || EWOULDBLOCK == errno)) {
        LOGW("try again, errno: %d(%s) \n", errno, strerror(errno));
        goto _FILE_WRITE_AGAIN;
    } else if (ret == -1) {
        LOGES("opposite fd close, fd: %d \n", fd);
        return -1;
    } else {
        return ret;
    }
}

hy_s32_t HyFileWriteN(hy_s32_t fd, const void *buf, hy_u32_t len)
{
    hy_s32_t ret;
    hy_u32_t nleft;
    const void *ptr;

    ptr   = buf;
    nleft = len;

    while (nleft > 0) {
    _FILE_WRITEN_AGAIN:
        ret = write(fd, ptr, nleft);
        if (ret < 0 && (EINTR == errno || EAGAIN == errno || EWOULDBLOCK == errno)) {
            LOGW("try again, errno: %d(%s) \n", errno, strerror(errno));
            goto _FILE_WRITEN_AGAIN;
        } else if (ret == -1) {
            LOGES("opposite fd close, fd: %d \n", fd);
            return -1;
        } else {
            nleft -= ret;
            ptr   += ret;
        }
    }

    return len;
}

static hy_s32_t _set_fcntl(hy_s32_t fd, hy_s32_t arg)
{
    hy_s32_t flags;

    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        flags = 0;
    }

    return fcntl(fd, F_SETFL, flags | arg);
}

hy_s32_t HyFileBlockStateSet(hy_s32_t fd, HyFileBlockState_e state)
{
    hy_s32_t flags;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        LOGE("fcntl failed \n");
        return -1;
    }

    if (HY_FILE_BLOCK_STATE_BLOCK == state) {
        return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
    } else {
        return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
}

HyFileBlockState_e HyFileBlockStateGet(hy_s32_t fd)
{
    hy_s32_t flag;

    flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0) {
        LOGES("fcntl failed \n");
    }

    if (flag & O_NONBLOCK) {
        return HY_FILE_BLOCK_STATE_NOBLOCK;
    } else {
        return HY_FILE_BLOCK_STATE_BLOCK;
    }
}

/*
 * 作用: 当fork子进程后，仍然可以使用fd。
 *       但执行exec后系统就会自动关闭进程中的fd
 */
hy_s32_t file_close_on_exec(hy_s32_t fd)
{
    return _set_fcntl(fd, FD_CLOEXEC);
}

