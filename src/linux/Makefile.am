# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    30/10 2021 11:44
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        30/10 2021      create the file
# 
#     last modified: 30/10 2021 11:44
# ===============================================================

SUBDIRS = \
          ipc   \
          net   \
          timer

noinst_LTLIBRARIES = libhy_utils_os.la

# ------------------
# defining variables
# ------------------
_no_error_com   =
_define_com     =
_include_com    = $(INCLUDE_COM)
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
                   -I$(top_srcdir)/include/$(run_os) \
                   -I$(top_srcdir)/include

_sources_com    += \
                   hy_fifo.c            \
                   hy_hash.c            \
                   hy_thread_pool.c     \
                   hy_timer.c           \
                   hy_utils_linux.c

if COMPILE_LIBLOG4CPLUS
_sources_com    += hy_log4cplus.cpp
else
_sources_com    +=
endif

if COMPILE_LIBCURL
_sources_com    += hy_curl.c
endif

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com)  \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    +=

_ldadd_com      += \
                   $(top_builddir)/src/$(run_os)/ipc/libhy_ipc.la       \
                   $(top_builddir)/src/$(run_os)/net/libhy_net.la       \
                   $(top_builddir)/src/$(run_os)/timer/libhy_timer.la   \
                   $(ldadd_com)

libhy_utils_os_la_SOURCES    = $(_sources_com)
libhy_utils_os_la_CPPFLAGS   = $(_cppflags_com)
libhy_utils_os_la_CFLAGS     = $(_cflags_com)
libhy_utils_os_la_CXXFLAGS   = $(_cxxflags_com)
libhy_utils_os_la_LDFLAGS    = $(_ldflags_com)
libhy_utils_os_la_LIBADD     = $(_ldadd_com)

