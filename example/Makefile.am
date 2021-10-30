# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    05/03 2021 10:38
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        05/03 2021      create the file
# 
#     last modified: 05/03 2021 10:38
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

_include_com    += -I$(top_srcdir)/include

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

if COMPILE_SELECT_TARGET_OS_LINUX
bin_PROGRAMS                += hy_net_wired_test
hy_net_wired_test_SOURCES   = $(_sources_com) hy_net_wired_test.c
hy_net_wired_test_CPPFLAGS  = $(_cppflags_com)
hy_net_wired_test_CFLAGS    = $(_cflags_com)
hy_net_wired_test_CXXFLAGS  = $(_cxxflags_com)
hy_net_wired_test_LDFLAGS   = $(_ldflags_com)
hy_net_wired_test_LDADD     = $(_ldadd_com)
endif

bin_PROGRAMS                += hy_template_test
hy_template_test_SOURCES    = $(_sources_com) hy_template_test.c
hy_template_test_CPPFLAGS   = $(_cppflags_com)
hy_template_test_CFLAGS     = $(_cflags_com)
hy_template_test_CXXFLAGS   = $(_cxxflags_com)
hy_template_test_LDFLAGS    = $(_ldflags_com)
hy_template_test_LDADD      = $(_ldadd_com)

bin_PROGRAMS                += hy_fifo_test
hy_fifo_test_SOURCES        = $(_sources_com) hy_fifo_test.c
hy_fifo_test_CPPFLAGS       = $(_cppflags_com)
hy_fifo_test_CFLAGS         = $(_cflags_com)
hy_fifo_test_CXXFLAGS       = $(_cxxflags_com)
hy_fifo_test_LDFLAGS        = $(_ldflags_com)
hy_fifo_test_LDADD          = $(_ldadd_com)

bin_PROGRAMS                += hy_list_test
hy_list_test_SOURCES        = $(_sources_com) hy_list_test.c
hy_list_test_CPPFLAGS       = $(_cppflags_com)
hy_list_test_CFLAGS         = $(_cflags_com)
hy_list_test_CXXFLAGS       = $(_cxxflags_com)
hy_list_test_LDFLAGS        = $(_ldflags_com)
hy_list_test_LDADD          = $(_ldadd_com)

bin_PROGRAMS                += main
main_SOURCES                = $(_sources_com) main.c
main_CPPFLAGS               = $(_cppflags_com)
main_CFLAGS                 = $(_cflags_com)
main_CXXFLAGS               = $(_cxxflags_com)
main_LDFLAGS                = $(_ldflags_com)
main_LDADD                  = $(_ldadd_com)

