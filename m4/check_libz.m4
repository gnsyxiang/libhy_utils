dnl ===============================================================
dnl 
dnl Release under GPLv-3.0.
dnl 
dnl @file    check_libz.m4
dnl @brief   
dnl @author  gnsyxiang <gnsyxiang@163.com>
dnl @date    28/03 2022 20:21
dnl @version v0.0.1
dnl 
dnl @since    note
dnl @note     note
dnl 
dnl     change log:
dnl     NO.     Author              Date            Modified
dnl     00      zhenquan.qiu        28/03 2022      create the file
dnl 
dnl     last modified: 28/03 2022 20:21
dnl ===============================================================

# CHECK_LIBZ()
# --------------------------------------------------------------
# check liblibz

AC_DEFUN([CHECK_LIBZ],
    [

        AC_ARG_ENABLE([libz],
            [AS_HELP_STRING([--enable-libz], [enable support for libz])],
            [],
            [enable_libz=no])

        case "$enable_libz" in
            yes)
                have_libz=no

                case "$PKG_CONFIG" in
                    '') ;;
                    *)
                        LIBZ_LIBS=`$PKG_CONFIG --libs zlib 2>/dev/null`

                        case "$LIBZ_LIBS" in
                            '') ;;
                            *)
                                LIBZ_LIBS="$LIBZ_LIBS"
                                LIBZ_INCS=`$PKG_CONFIG --cflags zlib 2>/dev/null`
                                have_libz=yes
                            ;;
                        esac
                    ;;
                esac

                case "$have_libz" in
                    yes) ;;
                    *)
                        save_LIBS="$LIBS"
                        LIBS=""
                        LIBZ_LIBS=""

                        # clear cache
                        unset ac_cv_search_zlibVersion
                        AC_SEARCH_LIBS([zlibVersion],
                                [libz],
                                [have_libz=yes LIBZ_LIBS="$LIBS"],
                                [have_libz=no],
                                [])
                        LIBS="$save_LIBS"
                    ;;
                esac

                CPPFLAGS_SAVE=$CPPFLAGS
                CPPFLAGS="$CPPFLAGS $LIBZ_INCS"
                AC_CHECK_HEADERS([zlib.h], [], [have_libz=no])

                CPPFLAGS=$CPPFLAGS_SAVE
                AC_SUBST(LIBZ_INCS)
                AC_SUBST(LIBZ_LIBS)

                case "$have_libz" in
                    yes)
                        AC_CHECK_LIB([libz], [zlibVersion])
                        AC_DEFINE(HAVE_LIBZ, 1, [Define if the system has libz])
                    ;;
                    *)
                        AC_MSG_ERROR([libz is a must but can not be found. You should add the \
directory containing `libz.pc' to the `PKG_CONFIG_PATH' environment variable, \
or set `CPPFLAGS' and `LDFLAGS' directly for libz, or use `--disable-libz' \
to disable support for libz encryption])
                    ;;
                esac
            ;;
        esac

        # check if we have and should use libz
        AM_CONDITIONAL(COMPILE_LIBZ, [test "$enable_libz" != "no" && test "$have_libz" = "yes"])
    ])

