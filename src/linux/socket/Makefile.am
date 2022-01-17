# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    17/01 2022 09:25
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        17/01 2022      create the file
# 
#     last modified: 17/01 2022 09:25
# ===============================================================

noinst_LTLIBRARIES = libhy_socket.la

# ------------------
# defining variables
# ------------------
_no_error_com   =
_define_com     =
_include_com    =
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
                   -I$(top_srcdir)/include/linux    \
                   -I$(top_srcdir)/include

_sources_com    += \
                   hy_socket.c          \
                   hy_socket_inside.c   \
                   hy_socket_server.c

if COMPILE_LIBLOG4CPLUS
_sources_com    += hy_log4cplus.cpp
else
_sources_com    +=
endif

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com)  \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    +=

_ldadd_com      +=

if COMPILE_LIBHY_HAL
_include_com    += $(HY_HAL_INCS)
endif

libhy_socket_la_SOURCES     = $(_sources_com)
libhy_socket_la_CPPFLAGS    = $(_cppflags_com)
libhy_socket_la_CFLAGS      = $(_cflags_com)
libhy_socket_la_CXXFLAGS    = $(_cxxflags_com)
libhy_socket_la_LDFLAGS     = $(_ldflags_com)
libhy_socket_la_LIBADD      = $(_ldadd_com)
