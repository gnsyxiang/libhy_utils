# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    15/05 2023 10:46
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        15/05 2023      create the file
# 
#     last modified: 15/05 2023 10:46
# ===============================================================

noinst_LTLIBRARIES = libhy_fifo.la

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

_define_com     += -D_GNU_SOURCE

_include_com    += \
                   -I$(top_srcdir)/include/$(run_os)/thread \
                   -I$(top_srcdir)/include/$(run_os)/fifo \
                   -I$(top_srcdir)/include/$(run_os) \
                   -I$(top_srcdir)/include

_sources_com    += \
                   hy_fifo_lock.c \
                   hy_fifo_lock_discard.c

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com) \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    +=

_ldadd_com      += $(lib_com)

libhy_fifo_la_SOURCES       = $(_sources_com)
libhy_fifo_la_CPPFLAGS      = $(_cppflags_com)
libhy_fifo_la_CFLAGS        = $(_cflags_com)
libhy_fifo_la_CXXFLAGS      = $(_cxxflags_com)
libhy_fifo_la_LDFLAGS       = $(_ldflags_com)
libhy_fifo_la_LIBADD        = $(_ldadd_com)
