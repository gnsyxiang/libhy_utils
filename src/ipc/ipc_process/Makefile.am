# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    03/03 2022 10:14
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        03/03 2022      create the file
# 
#     last modified: 03/03 2022 10:14
# ===============================================================

noinst_LTLIBRARIES = libhy_ipc_process.la

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
bin_PROGRAMS    =

# ---------------------
# add necessary content
# ---------------------
_no_error_com   +=

_define_com     += -Wno-error=strict-aliasing

_include_com    += \
                   -I$(top_srcdir)/include/ipc/ipc_socket     \
                   -I$(top_srcdir)/include/ipc/ipc_process    \
                   -I$(top_srcdir)/include/thread \
                   -I$(top_srcdir)/include

_sources_com    += \
                   hy_ipc_process.c

_sources_com    += \
                   ipc_link.c           \
                   ipc_link_manager.c   \
                   ipc_process_client.c \
                   ipc_process_server.c

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com)  \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    +=

_ldadd_com      += $(ldadd_com)

libhy_ipc_process_la_SOURCES    = $(_sources_com)
libhy_ipc_process_la_CPPFLAGS   = $(_cppflags_com)
libhy_ipc_process_la_CFLAGS     = $(_cflags_com)
libhy_ipc_process_la_CXXFLAGS   = $(_cxxflags_com)
libhy_ipc_process_la_LDFLAGS    = $(_ldflags_com)
libhy_ipc_process_la_LIBADD     = $(_ldadd_com)

