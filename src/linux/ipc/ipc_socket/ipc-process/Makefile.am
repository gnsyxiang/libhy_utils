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
_include_com    =
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

_define_com     +=

_include_com    += \
                   -I$(top_srcdir)/include/linux/ipc/ipc_socket/ipc-process     \
                   -I$(top_srcdir)/include/linux/ipc/ipc_socket                 \
                   -I$(top_srcdir)/include/linux                                \
                   -I$(top_srcdir)/include

_sources_com    += \
                   hy_ipc_process.c

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

libhy_ipc_process_la_SOURCES        = $(_sources_com)
libhy_ipc_process_la_CPPFLAGS       = $(_cppflags_com)
libhy_ipc_process_la_CFLAGS         = $(_cflags_com)
libhy_ipc_process_la_CXXFLAGS       = $(_cxxflags_com)
libhy_ipc_process_la_LDFLAGS        = $(_ldflags_com)
libhy_ipc_process_la_LIBADD         = $(_ldadd_com)

bin_PROGRAMS                        += hy_ipc_link_client_test
hy_ipc_link_client_test_SOURCES     = $(_sources_com) ipc_link_client_test.c
hy_ipc_link_client_test_CPPFLAGS    = $(_cppflags_com)
hy_ipc_link_client_test_CFLAGS      = $(_cflags_com)
hy_ipc_link_client_test_CXXFLAGS    = $(_cxxflags_com)
hy_ipc_link_client_test_LDFLAGS     = $(_ldflags_com)
hy_ipc_link_client_test_LDADD       = $(_ldadd_com)
