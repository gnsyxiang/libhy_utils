# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    check_libevent.m4
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    27/04 2023 10:16
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        27/04 2023      create the file
# 
#     last modified: 27/04 2023 10:16
# ===============================================================

AC_DEFUN([CHECK_LIBEVENT],
    [

        AC_ARG_ENABLE([event],
            [AS_HELP_STRING([--enable-event],
                [enable support for event])],
                [],
                [enable_event=no])

        case "$enable_event" in
            yes)
                have_event=no

                case "$PKG_CONFIG" in
                    '') ;;
                    *)
                        EVENT_LIBS=`$PKG_CONFIG --libs event 2>/dev/null`

                        case "$EVENT_LIBS" in
                            '') ;;
                            *)
                                EVENT_LIBS="$EVENT_LIBS"
                                EVENT_INCS=`$PKG_CONFIG --cflags event 2>/dev/null`
                                have_event=yes
                            ;;
                        esac
                    ;;
                esac

                case "$have_event" in
                    yes) ;;
                    *)
                        save_LIBS="$LIBS"
                        LIBS=""
                        EVENT_LIBS=""

                        # clear cache
                        unset ac_cv_search_event_base_new
                        AC_SEARCH_LIBS([event_base_new],
                                [event],
                                [have_event=yes EVENT_LIBS="$LIBS"],
                                [have_event=no],
                                [])
                        LIBS="$save_LIBS"
                    ;;
                esac

                CPPFLAGS_SAVE=$CPPFLAGS
                CPPFLAGS="$CPPFLAGS $EVENT_INCS"
                AC_CHECK_HEADERS([event2/event.h], [], [have_event=no])

                CPPFLAGS=$CPPFLAGS_SAVE
                AC_SUBST(EVENT_INCS)
                AC_SUBST(EVENT_LIBS)

                case "$have_event" in
                    yes)
                        AC_CHECK_LIB([event], [event_base_new])
                        AC_DEFINE(HAVE_EVENT, 1, [Define if the system has event])
                    ;;
                    *)
                        AC_MSG_ERROR([event is a must but can not be found. You should add the \
directory containing `event.pc' to the `PKG_CONFIG_PATH' environment variable, \
or set `CPPFLAGS' and `LDFLAGS' directly for event, or use `--disable-event' \
to disable support for event encryption])
                    ;;
                esac
            ;;
        esac

        # check if we have and should use event
        AM_CONDITIONAL(COMPILE_LIBEVENT, [test "$enable_event" != "no" && test "$have_event" = "yes"])
    ])

