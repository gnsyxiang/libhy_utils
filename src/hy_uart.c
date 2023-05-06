/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_uart.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    06/05 2023 15:38
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        06/05 2023      create the file
 * 
 *     last modified: 06/05 2023 15:38
 */
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "hy_assert.h"
#include "hy_file.h"
#include "hy_mem.h"

#include "hy_uart.h"

#define _HY_UART_READ_VMIN_LEN      (16)
#define _HY_UART_READ_VTIME_100MS   (10)

struct HyUart_s {
    HyUartSaveConfig_s  save_c;

    hy_s32_t            fd;
};

static hy_s32_t _uart_set_param(hy_s32_t fd, HyUartSaveConfig_s *save_c)
{
    struct termios options;

    if (tcgetattr(fd, &options)) {
        LOGES("tcgetattr faild \n");
        return -1;
    }

    // 配置为原始模式 (两种方式任选其一)
#if 0
    cfmakeraw(&options);
#else
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP| INLCR | IGNCR | ICRNL | IXON);
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    options.c_cflag &= ~(CSIZE | PARENB);
    options.c_cflag |= CS8;
#endif

    options.c_cflag |= (CLOCAL | CREAD); // CLOCAL和CREAD分别用于本地连接和接收使能

    // 设置波特率
    speed_t speed_2_speed[] = {
        B1200,
        B2400,
        B4800,
        B9600,
        B19200,
        B38400,
        B57600,
        B115200,
        B230400,
        B460800,
        B500000,
        B576000,
        B921600,
        B1000000,
        B1152000,
        B1500000,
        B2000000,
        B2500000,
        B3000000,
        B3500000,
        B4000000,
    };
    cfsetispeed(&options, speed_2_speed[save_c->speed]);
    cfsetospeed(&options, speed_2_speed[save_c->speed]);

    // 设置数据位
    hy_s32_t data_bit_2_bit[] = {
        CS5,
        CS6,
        CS7,
        CS8
    };
    options.c_cflag &= ~CSIZE;  // 清零
    options.c_cflag |= data_bit_2_bit[save_c->data_bit];

    // 设置校验位
    switch (save_c->parity_type) {
        case HY_UART_PARITY_NONE:
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
        break;
        case HY_UART_PARITY_ODD:
            options.c_cflag |= (PARENB | PARODD);
            options.c_iflag |= INPCK;
        break;
        case HY_UART_PARITY_EVEN:
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
        break;
        default:
            LOGE("the parity_type is error \n");
            break;
    }

    // 设置停止位
    switch (save_c->stop_bit) {
        case HY_UART_STOP_BIT_1:
            options.c_cflag &= ~CSTOPB;
            break;
        case HY_UART_STOP_BIT_2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            LOGE("the stop bit is error \n");
            break;
    }

    // 设置数据流控
    switch (save_c->flow_control) {
        case HY_UART_FLOW_CONTROL_NONE:
            options.c_cflag &= ~CRTSCTS;
            break;
        case HY_UART_FLOW_CONTROL_HARDWARE:
            options.c_cflag |= CRTSCTS;
            break;
        case HY_UART_FLOW_CONTROL_SOFT:
            options.c_cflag |= IXON | IXOFF | IXANY;
            break;
        default:
            LOGE("the flow_control is error \n"); 
            break;
    }

    // read阻塞条件: wait time and minmum number of "bytes"
    options.c_cc[VTIME]= _HY_UART_READ_VTIME_100MS; // wait for 0.1s
    options.c_cc[VMIN]= _HY_UART_READ_VMIN_LEN;     // read at least 1 byte

    // TCIFLUSH刷清输入队列
    // TCOFLUSH刷清输出队列
    // TCIOFLUSH刷清输入、输出队列
    tcflush (fd, TCIOFLUSH);

    // TCSANOW立即生效,
    // TCSADRAIN：Wait until everything has been transmitted；
    // TCSAFLUSH：Flush input and output buffers and make the change
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        LOGES("tty set error!\n");
        return -1;
    }

    return 0;
}

hy_s32_t HyUartWrite(HyUart_s *handle, const void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    return HyFileWrite(handle->fd, buf, len);
}

hy_s32_t HyUartWriteN(HyUart_s *handle, const void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    return HyFileWriteN(handle->fd, buf, len);

}

hy_s32_t HyUartRead(HyUart_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    return HyFileRead(handle->fd, buf, len);
}

hy_s32_t HyUartReadN(HyUart_s *handle, void *buf, hy_u32_t len)
{
    HY_ASSERT(handle);
    HY_ASSERT(buf);

    return HyFileReadN(handle->fd, buf, len);
}

void HyUartDestroy(HyUart_s **handle_pp)
{
    HY_ASSERT_RET(!handle_pp || !*handle_pp);
    HyUart_s *handle = *handle_pp;

    close(handle->fd);

    LOGI("uart destroy, handle: %p \n", handle);
    HY_MEM_FREE_PP(handle_pp);
}

HyUart_s *HyUartCreate(HyUartConfig_s *uart_c)
{
    HY_ASSERT_RET_VAL(!uart_c, NULL);
    HyUart_s *handle = NULL;

    do {
        handle = HY_MEM_CALLOC_BREAK(HyUart_s *, sizeof(*handle));
        HY_MEMCPY(&handle->save_c, &uart_c->save_c, sizeof(handle->save_c));

        hy_s32_t flags = O_RDWR | O_NOCTTY;
        // hy_s32_t flags = O_RDWR | O_NOCTTY | O_NDELAY;
        handle->fd = open(uart_c->save_c.dev_path, flags);
        if (handle->fd <= 0) {
            LOGES("open %s faild \n", uart_c->save_c.dev_path);
            break;
        }

        if (0 != _uart_set_param(handle->fd, &handle->save_c)) {
            LOGE("_uart_set_param failed \n");
            break;
        }

        LOGI("uart create, handle: %p \n", handle);
        return handle;
    } while(0);

    LOGE("uart create failed \n");
    HyUartDestroy(&handle);
    return NULL;
}
