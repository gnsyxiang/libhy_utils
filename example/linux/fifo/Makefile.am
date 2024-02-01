# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    15/05 2023 10:44
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        15/05 2023      create the file
# 
#     last modified: 15/05 2023 10:44
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
                   -I$(top_srcdir)/include/$(run_os)/thread \
                   -I$(top_srcdir)/include/$(run_os)/fifo \
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
                   $(top_builddir)/src/lib$(project_name).la

if COMPILE_LIBHY_OS
_ldadd_com      += $(HY_OS_LIBS)
endif

if COMPILE_LIBHY_OS_TYPE
_ldadd_com      += $(HY_OS_TYPE_LIBS)
endif

bin_PROGRAMS                        += hy_fifo_demo
hy_fifo_demo_SOURCES                = $(_sources_com) hy_fifo_demo.c
hy_fifo_demo_CPPFLAGS               = $(_cppflags_com)
hy_fifo_demo_CFLAGS                 = $(_cflags_com)
hy_fifo_demo_CXXFLAGS               = $(_cxxflags_com)
hy_fifo_demo_LDFLAGS                = $(_ldflags_com)
hy_fifo_demo_LDADD                  = $(_ldadd_com)

bin_PROGRAMS                        += hy_fifo_lock_demo
hy_fifo_lock_demo_SOURCES           = $(_sources_com) hy_fifo_lock_demo.c
hy_fifo_lock_demo_CPPFLAGS          = $(_cppflags_com)
hy_fifo_lock_demo_CFLAGS            = $(_cflags_com)
hy_fifo_lock_demo_CXXFLAGS          = $(_cxxflags_com)
hy_fifo_lock_demo_LDFLAGS           = $(_ldflags_com)
hy_fifo_lock_demo_LDADD             = $(_ldadd_com)

bin_PROGRAMS                        += hy_fifo_lock_discard_demo
hy_fifo_lock_discard_demo_SOURCES   = $(_sources_com) hy_fifo_lock_discard_demo.c
hy_fifo_lock_discard_demo_CPPFLAGS  = $(_cppflags_com)
hy_fifo_lock_discard_demo_CFLAGS    = $(_cflags_com)
hy_fifo_lock_discard_demo_CXXFLAGS  = $(_cxxflags_com)
hy_fifo_lock_discard_demo_LDFLAGS   = $(_ldflags_com)
hy_fifo_lock_discard_demo_LDADD     = $(_ldadd_com)
