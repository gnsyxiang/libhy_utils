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

SUBDIRS = \
          fifo \
          net \
          thread \
          time \
          uart

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
                   -I$(top_srcdir)/include/$(run_os)/time \
                   -I$(top_srcdir)/include/$(run_os)/net \
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

bin_PROGRAMS                    += hy_dir_demo
hy_dir_demo_SOURCES             = $(_sources_com) hy_dir_demo.c
hy_dir_demo_CPPFLAGS            = $(_cppflags_com)
hy_dir_demo_CFLAGS              = $(_cflags_com)
hy_dir_demo_CXXFLAGS            = $(_cxxflags_com)
hy_dir_demo_LDFLAGS             = $(_ldflags_com)
hy_dir_demo_LDADD               = $(_ldadd_com)

bin_PROGRAMS                    += hy_dynamic_array_demo
hy_dynamic_array_demo_SOURCES   = $(_sources_com) hy_dynamic_array_demo.c
hy_dynamic_array_demo_CPPFLAGS  = $(_cppflags_com)
hy_dynamic_array_demo_CFLAGS    = $(_cflags_com)
hy_dynamic_array_demo_CXXFLAGS  = $(_cxxflags_com)
hy_dynamic_array_demo_LDFLAGS   = $(_ldflags_com)
hy_dynamic_array_demo_LDADD     = $(_ldadd_com)

bin_PROGRAMS                    += hy_dynamic_lib_demo
hy_dynamic_lib_demo_SOURCES     = $(_sources_com) hy_dynamic_lib_demo.c
hy_dynamic_lib_demo_CPPFLAGS    = $(_cppflags_com)
hy_dynamic_lib_demo_CFLAGS      = $(_cflags_com)
hy_dynamic_lib_demo_CXXFLAGS    = $(_cxxflags_com)
hy_dynamic_lib_demo_LDFLAGS     = $(_ldflags_com)
hy_dynamic_lib_demo_LDADD       = $(_ldadd_com)

bin_PROGRAMS                    += hy_hash_demo
hy_hash_demo_SOURCES            = $(_sources_com) hy_hash_demo.c
hy_hash_demo_CPPFLAGS           = $(_cppflags_com)
hy_hash_demo_CFLAGS             = $(_cflags_com)
hy_hash_demo_CXXFLAGS           = $(_cxxflags_com)
hy_hash_demo_LDFLAGS            = $(_ldflags_com)
hy_hash_demo_LDADD              = $(_ldadd_com)

bin_PROGRAMS                    += hy_md5sum_demo
hy_md5sum_demo_SOURCES          = $(_sources_com) hy_md5sum_demo.c
hy_md5sum_demo_CPPFLAGS         = $(_cppflags_com)
hy_md5sum_demo_CFLAGS           = $(_cflags_com)
hy_md5sum_demo_CXXFLAGS         = $(_cxxflags_com)
hy_md5sum_demo_LDFLAGS          = $(_ldflags_com)
hy_md5sum_demo_LDADD            = $(_ldadd_com)

bin_PROGRAMS                    += hy_package_list_demo
hy_package_list_demo_SOURCES    = $(_sources_com) hy_package_list_demo.c
hy_package_list_demo_CPPFLAGS   = $(_cppflags_com)
hy_package_list_demo_CFLAGS     = $(_cflags_com)
hy_package_list_demo_CXXFLAGS   = $(_cxxflags_com)
hy_package_list_demo_LDFLAGS    = $(_ldflags_com)
hy_package_list_demo_LDADD      = $(_ldadd_com)

bin_PROGRAMS                    += hy_sort_demo
hy_sort_demo_SOURCES            = $(_sources_com) hy_sort_demo.c
hy_sort_demo_CPPFLAGS           = $(_cppflags_com)
hy_sort_demo_CFLAGS             = $(_cflags_com)
hy_sort_demo_CXXFLAGS           = $(_cxxflags_com)
hy_sort_demo_LDFLAGS            = $(_ldflags_com)
hy_sort_demo_LDADD              = $(_ldadd_com)

bin_PROGRAMS                    += hy_template_demo
hy_template_demo_SOURCES        = $(_sources_com) hy_template_demo.c
hy_template_demo_CPPFLAGS       = $(_cppflags_com)
hy_template_demo_CFLAGS         = $(_cflags_com)
hy_template_demo_CXXFLAGS       = $(_cxxflags_com)
hy_template_demo_LDFLAGS        = $(_ldflags_com)
hy_template_demo_LDADD          = $(_ldadd_com)
