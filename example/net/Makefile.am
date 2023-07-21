# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    01/04 2022 11:29
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        01/04 2022      create the file
# 
#     last modified: 01/04 2022 11:29
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
                   -I$(top_srcdir)/include/net \
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

bin_PROGRAMS                += hy_epoll_demo
hy_epoll_demo_SOURCES       = $(_sources_com) hy_epoll_demo.c
hy_epoll_demo_CPPFLAGS      = $(_cppflags_com)
hy_epoll_demo_CFLAGS        = $(_cflags_com)
hy_epoll_demo_CXXFLAGS      = $(_cxxflags_com)
hy_epoll_demo_LDFLAGS       = $(_ldflags_com)
hy_epoll_demo_LDADD         = $(_ldadd_com)

bin_PROGRAMS                += hy_net_demo
hy_net_demo_SOURCES         = $(_sources_com) hy_net_demo.c
hy_net_demo_CPPFLAGS        = $(_cppflags_com)
hy_net_demo_CFLAGS          = $(_cflags_com)
hy_net_demo_CXXFLAGS        = $(_cxxflags_com)
hy_net_demo_LDFLAGS         = $(_ldflags_com)
hy_net_demo_LDADD           = $(_ldadd_com)

bin_PROGRAMS                += hy_phy_led_demo
hy_phy_led_demo_SOURCES     = $(_sources_com) hy_phy_led_demo.c
hy_phy_led_demo_CPPFLAGS    = $(_cppflags_com)
hy_phy_led_demo_CFLAGS      = $(_cflags_com)
hy_phy_led_demo_CXXFLAGS    = $(_cxxflags_com)
hy_phy_led_demo_LDFLAGS     = $(_ldflags_com)
hy_phy_led_demo_LDADD       = $(_ldadd_com)

bin_PROGRAMS                += hy_client_demo
hy_client_demo_SOURCES     = $(_sources_com) hy_client_demo.c
hy_client_demo_CPPFLAGS    = $(_cppflags_com)
hy_client_demo_CFLAGS      = $(_cflags_com)
hy_client_demo_CXXFLAGS    = $(_cxxflags_com)
hy_client_demo_LDFLAGS     = $(_ldflags_com)
hy_client_demo_LDADD       = $(_ldadd_com)

bin_PROGRAMS                += hy_socket_demo
hy_socket_demo_SOURCES      = $(_sources_com) hy_socket_demo.c
hy_socket_demo_CPPFLAGS     = $(_cppflags_com)
hy_socket_demo_CFLAGS       = $(_cflags_com)
hy_socket_demo_CXXFLAGS     = $(_cxxflags_com)
hy_socket_demo_LDFLAGS      = $(_ldflags_com)
hy_socket_demo_LDADD        = $(_ldadd_com)
