dnl ===============================================================
dnl 
dnl Release under GPLv-3.0.
dnl 
dnl @file    check_libhy_hal.m4
dnl @brief   
dnl @author  gnsyxiang <gnsyxiang@163.com>
dnl @date    30/10 2021 10:22
dnl @version v0.0.1
dnl 
dnl @since    note
dnl @note     note
dnl 
dnl     change log:
dnl     NO.     Author              Date            Modified
dnl     00      zhenquan.qiu        30/10 2021      create the file
dnl 
dnl     last modified: 30/10 2021 10:22
dnl ===============================================================

# CHECK_LIBHY_HAL()
# --------------------------------------------------------------
# check libhy_hal

AC_DEFUN([CHECK_LIBHY_HAL], [

    AC_ARG_ENABLE([hy_hal],
        [AS_HELP_STRING([--disable-hy_hal], [disable support for hy_hal])],
            [], [enable_hy_hal=yes])

    case "$enable_hy_hal" in
        yes)
            have_hy_hal=no

            case "$PKG_CONFIG" in
                '') ;;
                *)
                    HY_HAL_LIBS=`$PKG_CONFIG --libs hy_hal 2>/dev/null`

                    case "$HY_HAL_LIBS" in
                        '') ;;
                        *)
                            HY_HAL_LIBS="$HY_HAL_LIBS"
                            have_hy_hal=yes
                        ;;
                    esac

                    HY_HAL_INCS=`$PKG_CONFIG --cflags hy_hal 2>/dev/null`
                ;;
            esac

            case "$have_hy_hal" in
                yes) ;;
                *)
                    save_LIBS="$LIBS"
                    LIBS=""
                    HY_HAL_LIBS=""

                    # clear cache
                    unset ac_cv_search_HyUartCreate
                    AC_SEARCH_LIBS([HyUartCreate], [hy_hal],
                            [have_hy_hal=yes
                                HY_HAL_LIBS="$LIBS"],
                            [have_hy_hal=no],
                            [])
                    LIBS="$save_LIBS"
                ;;
            esac

            CPPFLAGS_SAVE=$CPPFLAGS
            CPPFLAGS="$CPPFLAGS $HY_HAL_INCS"
            AC_CHECK_HEADERS([hy_hal/hy_uart.h], [], [have_hy_hal=no])

            CPPFLAGS=$CPPFLAGS_SAVE
            AC_SUBST(HY_HAL_INCS)
            AC_SUBST(HY_HAL_LIBS)

            case "$have_hy_hal" in
                yes)
                    AC_DEFINE(HAVE_HY_HAL, 1, [Define if the system has hy_hal])
                ;;
                *)
                    AC_MSG_ERROR([hy_hal is a must but can not be found. You should add the \
directory containing `hy_hal.pc' to the `PKG_CONFIG_PATH' environment variable, \
or set `CPPFLAGS' and `LDFLAGS' directly for hy_hal, or use `--disable-hy_hal' \
to disable support for hy_hal encryption])
                ;;
            esac
        ;;
    esac

    # check if we have and should use hy_hal
    AM_CONDITIONAL(COMPILE_LIBHY_HAL, [test "$enable_hy_hal" != "no" && test "$have_hy_hal" = "yes"])
])

