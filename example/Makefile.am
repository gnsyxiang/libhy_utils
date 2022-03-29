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

if COMPILE_LIBCJSON
bin_PROGRAMS                += hy_json_test
hy_json_test_SOURCES        = $(_sources_com) hy_json_test.c
hy_json_test_CPPFLAGS       = $(_cppflags_com)
hy_json_test_CFLAGS         = $(_cflags_com)
hy_json_test_CXXFLAGS       = $(_cxxflags_com)
hy_json_test_LDFLAGS        = $(_ldflags_com)
hy_json_test_LDADD          = $(_ldadd_com)
endif

bin_PROGRAMS                += hy_list_test
hy_list_test_SOURCES        = $(_sources_com) hy_list_test.c
hy_list_test_CPPFLAGS       = $(_cppflags_com)
hy_list_test_CFLAGS         = $(_cflags_com)
hy_list_test_CXXFLAGS       = $(_cxxflags_com)
hy_list_test_LDFLAGS        = $(_ldflags_com)
hy_list_test_LDADD          = $(_ldadd_com)

bin_PROGRAMS                += hy_md5sum_test
hy_md5sum_test_SOURCES      = $(_sources_com) hy_md5sum_test.c
hy_md5sum_test_CPPFLAGS     = $(_cppflags_com)
hy_md5sum_test_CFLAGS       = $(_cflags_com)
hy_md5sum_test_CXXFLAGS     = $(_cxxflags_com)
hy_md5sum_test_LDFLAGS      = $(_ldflags_com)
hy_md5sum_test_LDADD        = $(_ldadd_com)

bin_PROGRAMS                += hy_sort_test
hy_sort_test_SOURCES        = $(_sources_com) hy_sort_test.c
hy_sort_test_CPPFLAGS       = $(_cppflags_com)
hy_sort_test_CFLAGS         = $(_cflags_com)
hy_sort_test_CXXFLAGS       = $(_cxxflags_com)
hy_sort_test_LDFLAGS        = $(_ldflags_com)
hy_sort_test_LDADD          = $(_ldadd_com)

bin_PROGRAMS                += hy_utils_test
hy_utils_test_SOURCES       = $(_sources_com) hy_utils_test.c
hy_utils_test_CPPFLAGS      = $(_cppflags_com)
hy_utils_test_CFLAGS        = $(_cflags_com)
hy_utils_test_CXXFLAGS      = $(_cxxflags_com)
hy_utils_test_LDFLAGS       = $(_ldflags_com)
hy_utils_test_LDADD         = $(_ldadd_com)

