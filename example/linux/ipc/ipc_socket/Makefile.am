# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    19/01 2022 09:08
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        19/01 2022      create the file
# 
#     last modified: 19/01 2022 09:08
# ===============================================================

SUBDIRS = ipc_process

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
                   -I$(top_srcdir)/include/linux/ipc/ipc_socket     \
                   -I$(top_srcdir)/include/linux                    \
                   -I$(top_srcdir)/include

_sources_com    +=

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com)  \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    +=

_ldadd_com      += $(top_builddir)/src/lib@project_name@.la

if COMPILE_LIBHY_HAL
_include_com    += $(HY_HAL_INCS)
_ldadd_com      += $(HY_HAL_LIBS)
endif

if COMPILE_LIBLOG4CPLUS
_include_com    += $(LOG4CPLUS_INCS)
_ldadd_com      += $(LOG4CPLUS_LIBS)
endif

bin_PROGRAMS                        += hy_ipc_socket_client_test
hy_ipc_socket_client_test_SOURCES   = $(_sources_com) hy_ipc_socket_client_test.c
hy_ipc_socket_client_test_CPPFLAGS  = $(_cppflags_com)
hy_ipc_socket_client_test_CFLAGS    = $(_cflags_com)
hy_ipc_socket_client_test_CXXFLAGS  = $(_cxxflags_com)
hy_ipc_socket_client_test_LDFLAGS   = $(_ldflags_com)
hy_ipc_socket_client_test_LDADD     = $(_ldadd_com)

bin_PROGRAMS                        += hy_ipc_socket_server_test
hy_ipc_socket_server_test_SOURCES   = $(_sources_com) hy_ipc_socket_server_test.c
hy_ipc_socket_server_test_CPPFLAGS  = $(_cppflags_com)
hy_ipc_socket_server_test_CFLAGS    = $(_cflags_com)
hy_ipc_socket_server_test_CXXFLAGS  = $(_cxxflags_com)
hy_ipc_socket_server_test_LDFLAGS   = $(_ldflags_com)
hy_ipc_socket_server_test_LDADD     = $(_ldadd_com)