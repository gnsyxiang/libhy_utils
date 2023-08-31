# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    05/05 2023 09:17
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        05/05 2023      create the file
# 
#     last modified: 05/05 2023 09:17
# ===============================================================

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

_define_com     +=

_include_com    += \
                   -I$(top_srcdir)/include/$(run_os)/time \
                   -I$(top_srcdir)/include/$(run_os)/serialize \
                   -I$(top_srcdir)/include/$(run_os)/libevent \
                   -I$(top_srcdir)/include/$(run_os)/thread \
                   -I$(top_srcdir)/include/$(run_os) \
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
                   $(ldadd_com)

bin_PROGRAMS                            += hy_bufferevent_client_demo
hy_bufferevent_client_demo_SOURCES      = $(_sources_com) hy_bufferevent_client_demo.c
hy_bufferevent_client_demo_CPPFLAGS     = $(_cppflags_com)
hy_bufferevent_client_demo_CFLAGS       = $(_cflags_com)
hy_bufferevent_client_demo_CXXFLAGS     = $(_cxxflags_com)
hy_bufferevent_client_demo_LDFLAGS      = $(_ldflags_com)
hy_bufferevent_client_demo_LDADD        = $(_ldadd_com)

bin_PROGRAMS                            += hy_evconnlistener_demo
hy_evconnlistener_demo_SOURCES      = $(_sources_com) hy_evconnlistener_demo.c
hy_evconnlistener_demo_CPPFLAGS     = $(_cppflags_com)
hy_evconnlistener_demo_CFLAGS       = $(_cflags_com)
hy_evconnlistener_demo_CXXFLAGS     = $(_cxxflags_com)
hy_evconnlistener_demo_LDFLAGS      = $(_ldflags_com)
hy_evconnlistener_demo_LDADD        = $(_ldadd_com)

bin_PROGRAMS                        += hy_event_server_demo
hy_event_server_demo_SOURCES        = $(_sources_com) hy_event_server_demo.c collect_bends.c
hy_event_server_demo_CPPFLAGS       = $(_cppflags_com)
hy_event_server_demo_CFLAGS         = $(_cflags_com)
hy_event_server_demo_CXXFLAGS       = $(_cxxflags_com)
hy_event_server_demo_LDFLAGS        = $(_ldflags_com)
hy_event_server_demo_LDADD          = $(_ldadd_com)
