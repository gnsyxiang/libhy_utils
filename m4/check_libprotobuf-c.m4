dnl ===============================================================
dnl 
dnl Release under GPLv-3.0.
dnl 
dnl @file    check_libprotobuf-c.m4
dnl @brief   
dnl @author  gnsyxiang <gnsyxiang@163.com>
dnl @date    04/08 2021 19:57
dnl @version v0.0.1
dnl 
dnl @since    note
dnl @note     note
dnl 
dnl     change log:
dnl     NO.     Author              Date            Modified
dnl     00      zhenquan.qiu        04/08 2021      create the file
dnl 
dnl     last modified: 04/08 2021 19:57
dnl ===============================================================

# CHECK_LIBPROTOBUF_C()
# --------------------------------------------------------------
# check liblibprotobuf

AC_DEFUN([CHECK_LIBPROTOBUF_C],
    [

        AC_ARG_ENABLE([libprotobuf_c],
            [AS_HELP_STRING([--enable-libprotobuf_c], [enable support for libprotobuf_c])],
            [],
            [enable_libprotobuf_c=no])

        case "$enable_libprotobuf_c" in
            yes)
                have_libprotobuf_c=no

                case "$PKG_CONFIG" in
                    '') ;;
                    *)
                        LIBPROTOBUF_C_LIBS=`$PKG_CONFIG --libs libprotobuf_c 2>/dev/null`

                        case "$LIBPROTOBUF_C_LIBS" in
                            '') ;;
                            *)
                                LIBPROTOBUF_C_LIBS="$LIBPROTOBUF_C_LIBS"
                                LIBPROTOBUF_C_INCS=`$PKG_CONFIG --cflags libprotobuf_c 2>/dev/null`
                                have_libprotobuf_c=yes
                            ;;
                        esac
                    ;;
                esac

                case "$have_libprotobuf_c" in
                    yes) ;;
                    *)
                        save_LIBS="$LIBS"
                        LIBS=""
                        LIBPROTOBUF_C_LIBS=""

                        # clear cache
                        unset ac_cv_search_protobuf_c_version
                        AC_SEARCH_LIBS([protobuf_c_version],
                                [protobuf-c],
                                [have_libprotobuf_c=yes LIBPROTOBUF_C_LIBS="$LIBS"],
                                [have_libprotobuf_c=no],
                                [])
                        LIBS="$save_LIBS"
                    ;;
                esac

                CPPFLAGS_SAVE=$CPPFLAGS
                CPPFLAGS="$CPPFLAGS $LIBPROTOBUF_C_INCS"
                AC_CHECK_HEADERS([protobuf-c/protobuf-c.h], [], [have_libprotobuf_c=no])

                CPPFLAGS=$CPPFLAGS_SAVE
                AC_SUBST(LIBPROTOBUF_C_INCS)
                AC_SUBST(LIBPROTOBUF_C_LIBS)

                case "$have_libprotobuf_c" in
                    yes)
                        AC_CHECK_LIB([protobuf-c], [protobuf_c_version])
                        AC_DEFINE(HAVE_LIBPROTOBUF_C, 1, [Define if the system has libprotobuf_c])
                    ;;
                    *)
                        AC_MSG_ERROR([libprotobuf_c is a must but can not be found. You should add the \
directory containing `libprotobuf_c.pc' to the `PKG_CONFIG_PATH' environment variable, \
or set `CPPFLAGS' and `LDFLAGS' directly for libprotobuf_c, or use `--disable-libprotobuf_c' \
to disable support for libprotobuf_c encryption])
                    ;;
                esac

                AC_ARG_VAR([PROTOC_C], [protobuf-c compiler command])
                AC_PATH_PROG([PROTOC_C], [protoc-c], [],
                            [`$PKG_CONFIG --variable=exec_prefix libprotobuf-c`/bin:$PATH])

                if test -z "$PROTOC_C"; then
                  AC_MSG_ERROR([Please install the protobuf compiler from https://github.com/protobuf-c/protobuf-c])
                fi
            ;;
        esac

        # check if we have and should use libprotobuf_c
        AM_CONDITIONAL(COMPILE_LIBPROTOBUF_C, [test "$enable_libprotobuf_c" != "no" && test "$have_libprotobuf_c" = "yes"])
    ])

