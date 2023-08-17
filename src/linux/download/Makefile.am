# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    26/04 2023 09:39
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        26/04 2023      create the file
# 
#     last modified: 26/04 2023 09:39
# ===============================================================

noinst_LTLIBRARIES = libhy_download.la

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
                   -I$(top_srcdir)/include/$(run_os)/download \
                   -I$(top_srcdir)/include/$(run_os) \
                   -I$(top_srcdir)/include

_sources_com    +=

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

_ldadd_com      += $(ldadd_com)

libhy_download_la_SOURCES    = $(_sources_com)
libhy_download_la_CPPFLAGS   = $(_cppflags_com)
libhy_download_la_CFLAGS     = $(_cflags_com)
libhy_download_la_CXXFLAGS   = $(_cxxflags_com)
libhy_download_la_LDFLAGS    = $(_ldflags_com)
libhy_download_la_LIBADD     = $(_ldadd_com)

