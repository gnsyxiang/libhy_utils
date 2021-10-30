# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    30/10 2021 11:35
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        30/10 2021      create the file
# 
#     last modified: 30/10 2021 11:35
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
                   -I$(top_srcdir)/include/linux \
                   -I$(top_srcdir)/include

_sources_com    +=

_cppflags_com   += $(_no_error_com) $(_include_com) $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    +=

_ldadd_com      += $(top_builddir)/src/lib@project_name@.la

if COMPILE_LIBHY_HAL
_include_com    += $(HY_HAL_INCS)
_ldadd_com      += $(HY_HAL_LIBS)
endif

bin_PROGRAMS                += hy_timer_test
hy_timer_test_SOURCES       = $(_sources_com) hy_timer_test.c
hy_timer_test_CPPFLAGS      = $(_cppflags_com)
hy_timer_test_CFLAGS        = $(_cflags_com)
hy_timer_test_CXXFLAGS      = $(_cxxflags_com)
hy_timer_test_LDFLAGS       = $(_ldflags_com)
hy_timer_test_LDADD         = $(_ldadd_com)

bin_PROGRAMS                += hy_net_wired_test
hy_net_wired_test_SOURCES   = $(_sources_com) hy_net_wired_test.c
hy_net_wired_test_CPPFLAGS  = $(_cppflags_com)
hy_net_wired_test_CFLAGS    = $(_cflags_com)
hy_net_wired_test_CXXFLAGS  = $(_cxxflags_com)
hy_net_wired_test_LDFLAGS   = $(_ldflags_com)
hy_net_wired_test_LDADD     = $(_ldadd_com)

