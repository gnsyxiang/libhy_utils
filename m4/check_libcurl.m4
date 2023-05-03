dnl ===============================================================
dnl 
dnl Release under GPLv-3.0.
dnl 
dnl @file    check_libcurl.m4
dnl @brief   
dnl @author  gnsyxiang <gnsyxiang@163.com>
dnl @date    28/03 2022 20:56
dnl @version v0.0.1
dnl 
dnl @since    note
dnl @note     note
dnl 
dnl     change log:
dnl     NO.     Author              Date            Modified
dnl     00      zhenquan.qiu        28/03 2022      create the file
dnl 
dnl     last modified: 28/03 2022 20:56
dnl ===============================================================

# CHECK_LIBCURL()
# --------------------------------------------------------------
# check libcurl

AC_DEFUN([CHECK_LIBCURL],
    [

        AC_ARG_ENABLE([curl],
            [AS_HELP_STRING([--enable-curl],
                [enable support for curl])],
                [],
                [enable_curl=no])

        case "$enable_curl" in
            yes)
                have_curl=no

                case "$PKG_CONFIG" in
                    '') ;;
                    *)
                        CURL_LIBS=`$PKG_CONFIG --libs libcurl 2>/dev/null`

                        case "$CURL_LIBS" in
                            '') ;;
                            *)
                                CURL_LIBS="$CURL_LIBS"
                                CURL_INCS=`$PKG_CONFIG --cflags libcurl 2>/dev/null`
                                have_curl=yes
                            ;;
                        esac
                    ;;
                esac

                case "$have_curl" in
                    yes) ;;
                    *)
                        save_LIBS="$LIBS"
                        LIBS=""
                        CURL_LIBS=""

                        # clear cache
                        unset ac_cv_search_curl_version
                        AC_SEARCH_LIBS([curl_version],
                                [curl],
                                [have_curl=yes CURL_LIBS="$LIBS"],
                                [have_curl=no],
                                [])
                        LIBS="$save_LIBS"
                    ;;
                esac

                CPPFLAGS_SAVE=$CPPFLAGS
                CPPFLAGS="$CPPFLAGS $CURL_INCS"
                AC_CHECK_HEADERS([curl/curl.h], [], [have_curl=no])

                CPPFLAGS=$CPPFLAGS_SAVE
                AC_SUBST(CURL_INCS)
                AC_SUBST(CURL_LIBS)

                case "$have_curl" in
                    yes)
                        AC_CHECK_LIB([curl], [curl_version])
                        AC_DEFINE(HAVE_CURL, 1, [Define if the system has curl])
                    ;;
                    *)
                        AC_MSG_ERROR([curl is a must but can not be found. You should add the \
directory containing `libcurl.pc' to the `PKG_CONFIG_PATH' environment variable, \
or set `CPPFLAGS' and `LDFLAGS' directly for curl, or use `--disable-curl' \
to disable support for curl encryption])
                    ;;
                esac
            ;;
        esac

        # check if we have and should use curl
        AM_CONDITIONAL(COMPILE_LIBCURL, [test "$enable_curl" != "no" && test "$have_curl" = "yes"])
    ])

