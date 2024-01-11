# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    check_libhy_os_type.m4
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

AC_DEFUN([CHECK_LIBHY_OS_TYPE],
    [

        AC_ARG_ENABLE([hy_os_type],
            [AS_HELP_STRING([--disable-hy_os_type],
                [disable support for hy_os_type])],
                [],
                [enable_hy_os_type=yes])

        case "$enable_hy_os_type" in
            yes)
                have_hy_os_type=no

                case "$PKG_CONFIG" in
                    '') ;;
                    *)
                        HY_OS_TYPE_LIBS=`$PKG_CONFIG --libs hy_os_type 2>/dev/null`

                        case "$HY_OS_TYPE_LIBS" in
                            '') ;;
                            *)
                                HY_OS_TYPE_LIBS="$HY_OS_TYPE_LIBS"
                                HY_OS_TYPE_INCS=`$PKG_CONFIG --cflags hy_os_type 2>/dev/null`
                                have_hy_os_type=yes
                            ;;
                        esac
                    ;;
                esac

                case "$have_hy_os_type" in
                    yes) ;;
                    *)
                        save_LIBS="$LIBS"
                        LIBS=""
                        HY_OS_TYPE_LIBS=""

                        # clear cache
                        unset ac_cv_search_HyOsTypeCheck
                        AC_SEARCH_LIBS([HyOsTypeCheck],
                                [hy_os_type],
                                [have_hy_os_type=yes HY_OS_TYPE_LIBS="$LIBS"],
                                [have_hy_os_type=no],
                                [])
                        LIBS="$save_LIBS"
                    ;;
                esac

                CPPFLAGS_SAVE=$CPPFLAGS
                CPPFLAGS="$CPPFLAGS $HY_OS_TYPE_INCS"
                AC_CHECK_HEADERS([hy_os_type/hy_os_type.h], [], [have_hy_os_type=no])

                CPPFLAGS=$CPPFLAGS_SAVE
                AC_SUBST(HY_OS_TYPE_INCS)
                AC_SUBST(HY_OS_TYPE_LIBS)

                case "$have_hy_os_type" in
                    yes)
                        AC_DEFINE(HAVE_HY_OS_TYPE, 1, [Define if the system has hy_os_type])
                    ;;
                    *)
                        AC_MSG_ERROR([hy_os_type is a must but can not be found. You should add the \
directory containing `hy_os_type.pc' to the `PKG_CONFIG_PATH' environment variable, \
or set `CPPFLAGS' and `LDFLAGS' directly for hy_os_type, or use `--disable-hy_os_type' \
to disable support for hy_os_type encryption])
                    ;;
                esac
            ;;
        esac

        # check if we have and should use hy_os_type
        AM_CONDITIONAL(COMPILE_LIBHY_OS_TYPE, [test "$enable_hy_os_type" != "no" && test "$have_hy_os_type" = "yes"])
    ])
