# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    05/03 2021 10:35
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        05/03 2021      create the file
# 
#     last modified: 05/03 2021 10:35
# ===============================================================

noinst_LTLIBRARIES = lib@project_name@_@run_os@.la

SUBDIRS = \
          fifo \
          ipc \
          net \
          thread \
          time \
          uart

if COMPILE_LIBCURL
SUBDIRS += download
endif

if COMPILE_LIBEVENT
SUBDIRS += libevent
endif

if COMPILE_LIBCJSON
SUBDIRS += serialize
endif

# ------------------
# defining variables
# ------------------
_no_error_com   =
_define_com     =
_include_com    = $(include_com)
_sources_com    =
_cppflags_com   =
_cflags_com     =
_cxxflags_com   =
_ldflags_com    =
_ldadd_com      =

# ---------------------
# add necessary content
# ---------------------
_no_error_com   +=

_define_com     +=

_include_com    += \
                   -I$(top_srcdir)/include/$(run_os)/thread \
                   -I$(top_srcdir)/include/$(run_os) \
                   -I$(top_srcdir)/include

_sources_com    += \
                   hy_bit.c \
                   hy_can.c \
                   hy_dynamic_array.c \
                   hy_file.c \
                   hy_gpio.c \
                   hy_gpio_isr.c \
                   hy_hash.c \
                   hy_hex.c \
                   hy_md5sum.c \
                   hy_mem.c \
                   hy_package_list.c \
                   hy_pipe.c \
                   hy_queue.c \
                   hy_signal.c \
                   hy_sort.c \
                   hy_string.c \
                   hy_utils.c

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com)  \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldadd_com      += \
                   $(top_builddir)/src/$(run_os)/net/libhy_net.la \
                   $(top_builddir)/src/$(run_os)/thread/libhy_thread.la \
                   $(top_builddir)/src/$(run_os)/time/libhy_time.la \
                   $(top_builddir)/src/$(run_os)/fifo/libhy_fifo.la \
                   $(top_builddir)/src/$(run_os)/uart/libhy_uart.la

if COMPILE_LIBCJSON
_ldadd_com      += $(top_builddir)/src/$(run_Os)/serialize/libhy_serialize.la
endif

if COMPILE_LIBEVENT
_ldadd_com      += $(top_builddir)/src/$(run_Os)/libevent/libhy_event.la
endif

lib@project_name@_@run_os@_la_SOURCES    = $(_sources_com)
lib@project_name@_@run_os@_la_CPPFLAGS   = $(_cppflags_com)
lib@project_name@_@run_os@_la_CFLAGS     = $(_cflags_com)
lib@project_name@_@run_os@_la_CXXFLAGS   = $(_cxxflags_com)
lib@project_name@_@run_os@_la_LDFLAGS    = $(_ldflags_com)
lib@project_name@_@run_os@_la_LIBADD     = $(_ldadd_com)
