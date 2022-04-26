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

SUBDIRS = $(run_os)

# ------------------
# defining variables
# ------------------
_no_error_com   =
_define_com     =
_include_com    = $(INCLUDE_COM)
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

bin_PROGRAMS                += hy_hex_demo
hy_hex_demo_SOURCES         = $(_sources_com) hy_hex_demo.c
hy_hex_demo_CPPFLAGS        = $(_cppflags_com)
hy_hex_demo_CFLAGS          = $(_cflags_com)
hy_hex_demo_CXXFLAGS        = $(_cxxflags_com)
hy_hex_demo_LDFLAGS         = $(_ldflags_com)
hy_hex_demo_LDADD           = $(_ldadd_com)

if COMPILE_LIBCJSON
bin_PROGRAMS                += hy_json_demo
hy_json_demo_SOURCES        = $(_sources_com) hy_json_demo.c
hy_json_demo_CPPFLAGS       = $(_cppflags_com)
hy_json_demo_CFLAGS         = $(_cflags_com)
hy_json_demo_CXXFLAGS       = $(_cxxflags_com)
hy_json_demo_LDFLAGS        = $(_ldflags_com)
hy_json_demo_LDADD          = $(_ldadd_com)
endif

bin_PROGRAMS                += hy_md5sum_demo
hy_md5sum_demo_SOURCES      = $(_sources_com) hy_md5sum_demo.c
hy_md5sum_demo_CPPFLAGS     = $(_cppflags_com)
hy_md5sum_demo_CFLAGS       = $(_cflags_com)
hy_md5sum_demo_CXXFLAGS     = $(_cxxflags_com)
hy_md5sum_demo_LDFLAGS      = $(_ldflags_com)
hy_md5sum_demo_LDADD        = $(_ldadd_com)

bin_PROGRAMS                += hy_sort_demo
hy_sort_demo_SOURCES        = $(_sources_com) hy_sort_demo.c
hy_sort_demo_CPPFLAGS       = $(_cppflags_com)
hy_sort_demo_CFLAGS         = $(_cflags_com)
hy_sort_demo_CXXFLAGS       = $(_cxxflags_com)
hy_sort_demo_LDFLAGS        = $(_ldflags_com)
hy_sort_demo_LDADD          = $(_ldadd_com)

bin_PROGRAMS                += hy_template_demo
hy_template_demo_SOURCES    = $(_sources_com) hy_template_demo.c
hy_template_demo_CPPFLAGS   = $(_cppflags_com)
hy_template_demo_CFLAGS     = $(_cflags_com)
hy_template_demo_CXXFLAGS   = $(_cxxflags_com)
hy_template_demo_LDFLAGS    = $(_ldflags_com)
hy_template_demo_LDADD      = $(_ldadd_com)

bin_PROGRAMS                += hy_utils_demo
hy_utils_demo_SOURCES       = $(_sources_com) hy_utils_demo.c
hy_utils_demo_CPPFLAGS      = $(_cppflags_com)
hy_utils_demo_CFLAGS        = $(_cflags_com)
hy_utils_demo_CXXFLAGS      = $(_cxxflags_com)
hy_utils_demo_LDFLAGS       = $(_ldflags_com)
hy_utils_demo_LDADD         = $(_ldadd_com)

