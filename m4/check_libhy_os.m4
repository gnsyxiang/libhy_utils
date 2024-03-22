# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    check_libhy_os.m4
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    11/01 2024 10:25
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        11/01 2024      create the file
# 
#     last modified: 11/01 2024 10:25
# ===============================================================

AC_DEFUN([CHECK_LIBHY_OS],
    [

        AC_ARG_ENABLE([hy_os],
            [AS_HELP_STRING([--disable-hy_os],
                [disable support for hy_os])],
                [],
                [enable_hy_os=yes])

        case "$enable_hy_os" in
            yes)
                have_hy_os=no

                case "$PKG_CONFIG" in
                    '') ;;
                    *)
                        HY_OS_LIBS=`$PKG_CONFIG --libs hy_os 2>/dev/null`

                        case "$HY_OS_LIBS" in
                            '') ;;
                            *)
                                HY_OS_LIBS="$HY_OS_LIBS"
                                HY_OS_INCS=`$PKG_CONFIG --cflags hy_os 2>/dev/null`
                                have_hy_os=yes
                            ;;
                        esac
                    ;;
                esac

                case "$have_hy_os" in
                    yes) ;;
                    *)
                        save_LIBS="$LIBS"
                        LIBS=""
                        HY_OS_LIBS=""

                        # clear cache
                        unset ac_cv_search_HyOsCheck
                        AC_SEARCH_LIBS([HyOsCheck],
                                [hy_os],
                                [have_hy_os=yes HY_OS_LIBS="$LIBS"],
                                [have_hy_os=no],
                                [])
                        LIBS="$save_LIBS"
                    ;;
                esac

                CPPFLAGS_SAVE=$CPPFLAGS
                CPPFLAGS="$CPPFLAGS $HY_OS_INCS"
                AC_CHECK_HEADERS([hy_os/hy_os.h], [], [have_hy_os=no])

                CPPFLAGS=$CPPFLAGS_SAVE
                AC_SUBST(HY_OS_INCS)
                AC_SUBST(HY_OS_LIBS)

                case "$have_hy_os" in
                    yes)
                        AC_DEFINE(HAVE_HY_OS, 1, [Define if the system has hy_os])
                    ;;
                    *)
                        AC_MSG_ERROR([hy_os is a must but can not be found. You should add the \
directory containing `hy_os.pc' to the `PKG_CONFIG_PATH' environment variable, \
or set `CPPFLAGS' and `LDFLAGS' directly for hy_os, or use `--disable-hy_os' \
to disable support for hy_os encryption])
                    ;;
                esac
            ;;
        esac

        # check if we have and should use hy_os
        AM_CONDITIONAL(COMPILE_LIBHY_OS, [test "$enable_hy_os" != "no" && test "$have_hy_os" = "yes"])
    ])
