# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    03/03 2022 10:28
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        03/03 2022      create the file
# 
#     last modified: 03/03 2022 10:28
# ===============================================================

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
                   -I$(top_srcdir)/include/$(run_os)/ipc/ipc_process    \
                   -I$(top_srcdir)/include/$(run_os)                    \
                   -I$(top_srcdir)/include

_sources_com    +=

_cppflags_com   += \
                   $(_no_error_com) \
                   $(_include_com)  \
                   $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    +=

_ldadd_com      += \
                   $(top_builddir)/src/lib@project_name@.la \
                   $(LIB_COM)

if COMPILE_LIBHY_HAL
_include_com    += $(HY_HAL_INCS)
_ldadd_com      += $(HY_HAL_LIBS)
endif

if COMPILE_LIBCJSON
_include_com    += $(CJSON_INCS)
_ldadd_com      += $(CJSON_LIBS)
endif

if COMPILE_LIBLOG4CPLUS
_include_com    += $(LOG4CPLUS_INCS)
_ldadd_com      += $(LOG4CPLUS_LIBS)
endif

if COMPILE_DEBUG_INFO
bin_PROGRAMS                        += hy_ipc_link_test
hy_ipc_link_test_SOURCES            = $(_sources_com) ipc_link_test.c
hy_ipc_link_test_CPPFLAGS           = $(_cppflags_com)
hy_ipc_link_test_CFLAGS             = $(_cflags_com)
hy_ipc_link_test_CXXFLAGS           = $(_cxxflags_com)
hy_ipc_link_test_LDFLAGS            = $(_ldflags_com)
hy_ipc_link_test_LDADD              = $(_ldadd_com)

bin_PROGRAMS                        += hy_ipc_link_manager_test
hy_ipc_link_manager_test_SOURCES    = $(_sources_com) ipc_link_manager_test.c
hy_ipc_link_manager_test_CPPFLAGS   = $(_cppflags_com)
hy_ipc_link_manager_test_CFLAGS     = $(_cflags_com)
hy_ipc_link_manager_test_CXXFLAGS   = $(_cxxflags_com)
hy_ipc_link_manager_test_LDFLAGS    = $(_ldflags_com)
hy_ipc_link_manager_test_LDADD      = $(_ldadd_com)
endif

bin_PROGRAMS                        += hy_ipc_process_client_test
hy_ipc_process_client_test_SOURCES  = $(_sources_com) hy_ipc_process_client_test.c
hy_ipc_process_client_test_CPPFLAGS = $(_cppflags_com)
hy_ipc_process_client_test_CFLAGS   = $(_cflags_com)
hy_ipc_process_client_test_CXXFLAGS = $(_cxxflags_com)
hy_ipc_process_client_test_LDFLAGS  = $(_ldflags_com)
hy_ipc_process_client_test_LDADD    = $(_ldadd_com)

bin_PROGRAMS                            += hy_ipc_process_client_set_test
hy_ipc_process_client_set_test_SOURCES  = $(_sources_com) hy_ipc_process_client_set_test.c
hy_ipc_process_client_set_test_CPPFLAGS = $(_cppflags_com)
hy_ipc_process_client_set_test_CFLAGS   = $(_cflags_com)
hy_ipc_process_client_set_test_CXXFLAGS = $(_cxxflags_com)
hy_ipc_process_client_set_test_LDFLAGS  = $(_ldflags_com)
hy_ipc_process_client_set_test_LDADD    = $(_ldadd_com)

bin_PROGRAMS                        += hy_ipc_process_server_test
hy_ipc_process_server_test_SOURCES  = $(_sources_com) hy_ipc_process_server_test.c
hy_ipc_process_server_test_CPPFLAGS = $(_cppflags_com)
hy_ipc_process_server_test_CFLAGS   = $(_cflags_com)
hy_ipc_process_server_test_CXXFLAGS = $(_cxxflags_com)
hy_ipc_process_server_test_LDFLAGS  = $(_ldflags_com)
hy_ipc_process_server_test_LDADD    = $(_ldadd_com)

