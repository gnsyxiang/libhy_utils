dnl ===============================================================
dnl 
dnl Release under GPLv-3.0.
dnl 
dnl @file    check_libcjson.m4
dnl @brief   
dnl @author  gnsyxiang <gnsyxiang@163.com>
dnl @date    30/10 2021 19:51
dnl @version v0.0.1
dnl 
dnl @since    note
dnl @note     note
dnl 
dnl     change log:
dnl     NO.     Author              Date            Modified
dnl     00      zhenquan.qiu        30/10 2021      create the file
dnl 
dnl     last modified: 30/10 2021 19:51
dnl ===============================================================

# CHECK_LIBCJSON()
# --------------------------------------------------------------
# check libcjson

AC_DEFUN([CHECK_LIBCJSON],
    [
        AC_ARG_ENABLE([cjson],
            [AS_HELP_STRING([--enable-cjson], [enable support for cjson])],
            [],
            [enable_cjson=no])

        case "$enable_cjson" in
            yes)
                have_cjson=no

                case "$PKG_CONFIG" in
                    '') ;;
                    *)
                        CJSON_LIBS=`$PKG_CONFIG --libs cjson 2>/dev/null`

                        case "$CJSON_LIBS" in
                            '') ;;
                            *)
                                CJSON_LIBS="$CJSON_LIBS"
                                CJSON_INCS=`$PKG_CONFIG --cflags cjson 2>/dev/null`
                                have_cjson=yes
                            ;;
                        esac
                    ;;
                esac

                case "$have_cjson" in
                    yes) ;;
                    *)
                        save_LIBS="$LIBS"
                        LIBS=""
                        CJSON_LIBS=""

                        # clear cache
                        unset ac_cv_search_cJSON_Version
                        AC_SEARCH_LIBS([cJSON_Version],
                                [cjson],
                                [have_cjson=yes CJSON_LIBS="$LIBS"],
                                [have_cjson=no],
                                [])
                        LIBS="$save_LIBS"
                    ;;
                esac

                CPPFLAGS_SAVE=$CPPFLAGS
                CPPFLAGS="$CPPFLAGS $CJSON_INCS"
                AC_CHECK_HEADERS([cjson/cJSON.h], [], [have_cjson=no])

                CPPFLAGS=$CPPFLAGS_SAVE
                AC_SUBST(CJSON_INCS)
                AC_SUBST(CJSON_LIBS)

                case "$have_cjson" in
                    yes)
                        AC_DEFINE(HAVE_CJSON, 1, [Define if the system has cjson])
                    ;;
                    *)
                        AC_MSG_ERROR([cjson is a must but can not be found. You should add the \
directory containing `cjson.pc' to the `PKG_CONFIG_PATH' environment variable, \
or set `CPPFLAGS' and `LDFLAGS' directly for cjson, or use `--disable-cjson' \
to disable support for cjson encryption])
                    ;;
                esac
            ;;
        esac

        # check if we have and should use cjson
        AM_CONDITIONAL(COMPILE_LIBCJSON, [test "$enable_cjson" != "no" && test "$have_cjson" = "yes"])
    ])

