dnl ===============================================================
dnl 
dnl Release under GPLv-3.0.
dnl 
dnl @file    check_libssl.m4
dnl @brief   
dnl @author  gnsyxiang <gnsyxiang@163.com>
dnl @date    28/03 2022 20:44
dnl @version v0.0.1
dnl 
dnl @since    note
dnl @note     note
dnl 
dnl     change log:
dnl     NO.     Author              Date            Modified
dnl     00      zhenquan.qiu        28/03 2022      create the file
dnl 
dnl     last modified: 28/03 2022 20:44
dnl ===============================================================

# CHECK_LIBSSL()
# --------------------------------------------------------------
# check libssl

AC_DEFUN([CHECK_LIBSSL],
    [

        AC_ARG_ENABLE([ssl],
            [AS_HELP_STRING([--enable-ssl], [enable support for ssl])],
            [],
            [enable_ssl=no])

        case "$enable_ssl" in
            yes)
                have_ssl=no

                case "$PKG_CONFIG" in
                    '') ;;
                    *)
                        SSL_LIBS=`$PKG_CONFIG --libs libssl 2>/dev/null`

                        case "$SSL_LIBS" in
                            '') ;;
                            *)
                                SSL_LIBS="$SSL_LIBS"
                                SSL_INCS=`$PKG_CONFIG --cflags libssl 2>/dev/null`
                                have_ssl=yes
                            ;;
                        esac
                    ;;
                esac

                case "$have_ssl" in
                    yes) ;;
                    *)
                        save_LIBS="$LIBS"
                        LIBS=""
                        SSL_LIBS=""

                        # clear cache
                        unset ac_cv_search_SSL_in_init
                        AC_SEARCH_LIBS([SSL_in_init],
                                [ssl],
                                [have_ssl=yes SSL_LIBS="$LIBS"],
                                [have_ssl=no],
                                [])
                        LIBS="$save_LIBS"
                    ;;
                esac

                CPPFLAGS_SAVE=$CPPFLAGS
                CPPFLAGS="$CPPFLAGS $SSL_INCS"
                AC_CHECK_HEADERS([openssl/ssl.h], [], [have_ssl=no])

                CPPFLAGS=$CPPFLAGS_SAVE
                AC_SUBST(SSL_INCS)
                AC_SUBST(SSL_LIBS)

                case "$have_ssl" in
                    yes)
                        AC_DEFINE(HAVE_SSL, 1, [Define if the system has ssl])
                    ;;
                    *)
                        AC_MSG_ERROR([ssl is a must but can not be found. You should add the \
directory containing `libssl.pc' to the `PKG_CONFIG_PATH' environment variable, \
or set `CPPFLAGS' and `LDFLAGS' directly for ssl, or use `--disable-ssl' \
to disable support for ssl encryption])
                    ;;
                esac
            ;;
        esac

        # check if we have and should use ssl
        AM_CONDITIONAL(COMPILE_LIBSSL, [test "$enable_ssl" != "no" && test "$have_ssl" = "yes"])
    ])

