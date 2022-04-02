# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    01/04 2022 11:35
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        01/04 2022      create the file
# 
#     last modified: 01/04 2022 11:35
# ===============================================================

noinst_LTLIBRARIES = libhy_net.la

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
                   -I$(top_srcdir)/include/$(run_os)/net    \
                   -I$(top_srcdir)/include/$(run_os)        \
                   -I$(top_srcdir)/include

_sources_com    += \
                   hy_net.c             \
                   net_wifi_config.c    \
                   hy_net_wifi.c

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com)  \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    +=

_ldadd_com      += $(ldadd_com)

libhy_net_la_SOURCES    = $(_sources_com)
libhy_net_la_CPPFLAGS   = $(_cppflags_com)
libhy_net_la_CFLAGS     = $(_cflags_com)
libhy_net_la_CXXFLAGS   = $(_cxxflags_com)
libhy_net_la_LDFLAGS    = $(_ldflags_com)
libhy_net_la_LIBADD     = $(_ldadd_com)

