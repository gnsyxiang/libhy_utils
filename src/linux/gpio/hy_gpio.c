/**
 * 
 * Release under GPLv-3.0.
 * 
 * @file    hy_gpio.c
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    02/04 2022 15:13
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        02/04 2022      create the file
 * 
 *     last modified: 02/04 2022 15:13
 */
#include <stdio.h>
#include <unistd.h>

#include "hy_assert.h"
#include "hy_string.h"
#include "hy_mem.h"
#include "hy_gpio.h"

#define _GPIO_EXPORT_PATH       "/sys/class/gpio/export"
#define _GPIO_UNEXPORT_PATH     "/sys/class/gpio/unexport"
#define _GPIO_DIRECTION_PATH    "/sys/class/gpio/gpio%d/direction"
#define _GPIO_VALUE_PATH        "/sys/class/gpio/gpio%d/value"

#define _HY_GPIO_DIR_OUT        "out"
#define _HY_GPIO_DIR_IN         "in"

#define _EXPORT_GPIO(_gpio)                                             \
    do {                                                                \
        FILE *fp = fopen(_GPIO_EXPORT_PATH, "w");                       \
        if (!fp) {                                                      \
            LOGES("open %s failed", _GPIO_EXPORT_PATH);                 \
            return -1;                                                  \
        }                                                               \
        fprintf(fp, "%d", _gpio->gpio);                                 \
        fclose(fp);                                                     \
    } while (0)

#define _UNEXPORT_GPIO(_gpio)                                           \
    do {                                                                \
        FILE *fp = fopen(_GPIO_UNEXPORT_PATH, "w");                     \
        if (!fp) {                                                      \
            LOGES("open %s failed", _GPIO_UNEXPORT_PATH);               \
            return -1;                                                  \
        }                                                               \
        fprintf(fp, "%d", _gpio->gpio);                                 \
        fclose(fp);                                                     \
    } while (0)

#define _SET_DIRECTION(_gpio, _direction)                               \
    do {                                                                \
        char buf[64] = {0};                                             \
        FILE *fp = NULL;                                                \
        snprintf(buf, sizeof(buf), _GPIO_DIRECTION_PATH, _gpio->gpio);  \
        fp = fopen(buf, "w");                                           \
        if (!fp) {                                                      \
            LOGES("open %s failed", buf);                               \
            return -1;                                                  \
        }                                                               \
        if (_direction) {                                               \
            fprintf(fp, "%s", "out");                                   \
        } else {                                                        \
            fprintf(fp, "%s", "in");                                    \
        }                                                               \
        fclose(fp);                                                     \
    } while (0)

#define _SET_VAL(_gpio, _val)                                           \
    do {                                                                \
        FILE *fp = NULL;                                                \
        char buf[64] = {0};                                             \
        snprintf(buf, sizeof(buf), _GPIO_VALUE_PATH, _gpio->gpio);      \
        fp = fopen(buf, "w");                                           \
        if (!fp) {                                                      \
            LOGES("open %s failed", buf);                               \
            return -1;                                                  \
        }                                                               \
        if (HY_GPIO_VAL_ON == _val) {                                   \
            if (_gpio->active_val == HY_GPIO_ACTIVE_VAL_1) {            \
                fprintf(fp, "%d", 1);                                   \
            } else {                                                    \
                fprintf(fp, "%d", 0);                                   \
            }                                                           \
        } else {                                                        \
            if (_gpio->active_val == HY_GPIO_ACTIVE_VAL_1) {            \
                fprintf(fp, "%d", 0);                                   \
            } else {                                                    \
                fprintf(fp, "%d", 1);                                   \
            }                                                           \
        }                                                               \
        fclose(fp);                                                     \
    } while (0)

#define _GET_VAL(_gpio, _val)                                           \
    do {                                                                \
        char buf[64] = {0};                                             \
        char ch;                                                        \
        FILE *fp = NULL;                                                \
        snprintf(buf, sizeof(buf), _GPIO_VALUE_PATH, _gpio->gpio);      \
        fp = fopen(buf, "r");                                           \
        if (!fp) {                                                      \
            LOGES("open %s failed\n", buf);                             \
            return -1;                                                  \
        }                                                               \
        fread(&ch, 1, 1, fp);                                           \
        if (ch != '1') {                                                \
            if (_gpio->active_val == HY_GPIO_ACTIVE_VAL_1) {            \
                _val = 1;                                               \
            } else {                                                    \
                _val = 0;                                               \
            }                                                           \
        } else {                                                        \
            if (_gpio->active_val == HY_GPIO_ACTIVE_VAL_1) {            \
                _val = 0;                                               \
            } else {                                                    \
                _val = 1;                                               \
            }                                                           \
        }                                                               \
        fclose(fp);                                                     \
    } while (0)

hy_s32_t HyGpioSetOutputVal(HyGpio_s *gpio, HyGpioVal_e val)
{
    _EXPORT_GPIO(gpio);
    _SET_DIRECTION(gpio, HY_GPIO_DIRECTION_OUT);
    _SET_VAL(gpio, val);
    _UNEXPORT_GPIO(gpio);

    return 0;
}

hy_s32_t HyGpioSetDirection(HyGpio_s *gpio, HyGpioDirection_e direction)
{
    _EXPORT_GPIO(gpio);
    _SET_DIRECTION(gpio, direction);
    _UNEXPORT_GPIO(gpio);

    return 0;
}

hy_s32_t HyGpioSetVal(HyGpio_s *gpio, HyGpioVal_e val)
{
    _EXPORT_GPIO(gpio);
    _SET_VAL(gpio, val);
    _UNEXPORT_GPIO(gpio);

    return 0;
}

hy_s32_t HyGpioGetVal(HyGpio_s *gpio, HyGpioVal_e *val)
{
    _EXPORT_GPIO(gpio);
    _GET_VAL(gpio, *val);
    _UNEXPORT_GPIO(gpio);

    return 0;
}

static hy_s32_t _gpio_export(hy_u32_t gpio, const char *export) // export unexport
{
    char buf[64] = {0};
    hy_s32_t ret;
    FILE *fp;

    snprintf(buf, sizeof(buf), "%s/gpio%d", HY_GPIO_CLASS_PATH, gpio);

    if (0 == access(buf, F_OK)) {
        return 0;
    } else {
        HY_MEMSET(buf, sizeof(buf));
        snprintf(buf, sizeof(buf), "%s/%s", HY_GPIO_CLASS_PATH, export);

        fp = fopen(buf, "w");
        if (!fp) {
            LOGES("fopen failed, fd: %p \n", fp);
            return -1;
        }

        ret = fprintf(fp, "%d", gpio);
        if (ret < 0) {
            LOGES("fwrite failed, ret: %d \n", ret);
            fclose(fp);
            return -1;
        }

        fclose(fp);
        return 0;
    }
}

static hy_s32_t _gpio_config(hy_u32_t gpio, const char *attr, const char *val)
{
    char buf[64] = {0};
    FILE *fp;
    hy_s32_t ret;

    snprintf(buf, sizeof(buf), "%s/gpio%d/%s", HY_GPIO_CLASS_PATH, gpio, attr);

    fp = fopen(buf, "w");
    if (!fp) {
        LOGES("fopen failed, fd: %p \n", fp);
        return -1;
    }

    ret = fwrite(val, strlen(val), 1, fp);
    if (ret != 1) {
        LOGES("fwrite failed, ret: %d \n", ret);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

hy_s32_t HyGpioExport(hy_u32_t gpio, HyGpioExport_e export)
{
    hy_s32_t ret = 0;

    if (export == HY_GPIO_EXPORT) {
        ret = _gpio_export(gpio, "export");
    } else {
        ret = _gpio_export(gpio, "unexport");
    }

    return ret;
}

hy_s32_t HyGpioSetDirection2(hy_u32_t gpio, HyGpioDirection_e direction)
{
    hy_s32_t ret = 0;

    if (direction == HY_GPIO_DIRECTION_IN) {
        ret = _gpio_config(gpio, "direction", "in");
    } else {
        ret = _gpio_config(gpio, "direction", "out");
    }

    return ret;
}

hy_s32_t HyGpioSetActiveValue(hy_u32_t gpio, HyGpioActiveVal_e active_val)
{
    hy_s32_t ret = 0;

    if (active_val == HY_GPIO_ACTIVE_VAL_0) {
        ret = _gpio_config(gpio, "active_low", "0");
    } else {
        ret = _gpio_config(gpio, "active_low", "1");
    }

    return ret;
}

hy_s32_t HyGpioSetTrigger(hy_u32_t gpio, HyGpioTrigger_e trigger)
{
    hy_s32_t ret = 0;

    switch (trigger) {
        case HY_GPIO_TRIGGER_NONE:
            ret = _gpio_config(gpio, "edge", "none");
            break;
        case HY_GPIO_TRIGGER_RISING:
            ret = _gpio_config(gpio, "edge", "rising");
            break;
        case HY_GPIO_TRIGGER_FALLING:
            ret = _gpio_config(gpio, "edge", "falling");
            break;
        case HY_GPIO_TRIGGER_BOTH:
            ret = _gpio_config(gpio, "edge", "both");
            break;
    }
    return ret;
}

hy_s32_t HyGpioConfig(HyGpio_s *gpio)
{
    const char *direction[] = {"in", "out"};
    const char *active_val[] = {"0", "1"};
    const char *trigger[] = {"none", "rising", "falling", "both"};
    hy_s32_t ret = 0;

    const char *val[][2] = {
        {"direction",   direction[gpio->direction]},
        {"active_low",  active_val[gpio->active_val]},
        {"edge",        trigger[gpio->trigger]},
    };

    ret = _gpio_export(gpio->gpio, "export");
    if (ret < 0) {
        LOGE("gpio export failed \n");
        return -1;
    }

    for (size_t i = 0; i < sizeof(val) / sizeof(val[0]); i++) {
        ret = _gpio_config(gpio->gpio, val[i][0], val[i][1]);
        if (ret != 0) {
            LOGE("gpio config failed \n");
            break;
        }
    }

    return ret;
}

