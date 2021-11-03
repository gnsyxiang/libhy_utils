# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    05/03 2021 10:35
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        05/03 2021      create the file
# 
#     last modified: 05/03 2021 10:35
# ===============================================================

if COMPILE_SELECT_TARGET_OS_LINUX
select_target_os = linux
endif

if COMPILE_SELECT_TARGET_OS_MAC
select_target_os = mac
endif

if COMPILE_SELECT_TARGET_OS_WINDOW
select_target_os = window
endif

if COMPILE_SELECT_TARGET_OS_RTOS
select_target_os = rtos
endif

if COMPILE_SELECT_TARGET_OS_MCU
select_target_os = mcu
endif

SUBDIRS = $(select_target_os)

lib_LTLIBRARIES = lib@project_name@.la

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

# ---------------------
# add necessary content
# ---------------------
_no_error_com   +=

_define_com     +=

_include_com    += -I$(top_srcdir)/include

_sources_com    += \
                   hy_bit.c         \
                   hy_fifo.c        \
                   hy_md5sum.c      \
                   hy_sort.c        \
                   hy_utils.c

_cppflags_com   += $(_no_error_com) $(_include_com) $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    += \
                   -export-symbols-regex 'Hy*'                  \
                   -version-info @current@:@revision@:@age@

_ldadd_com      += $(top_builddir)/src/$(select_target_os)/libhy_utils_os.la

if COMPILE_LIBHY_HAL
_include_com    += $(HY_HAL_INCS)
endif

if COMPILE_LIBCJSON
_sources_com    += \
                   hy_json.c        \
                   hy_cjson_impl.c
_include_com    += $(CJSON_INCS)
endif

lib@project_name@_la_SOURCES    = $(_sources_com)
lib@project_name@_la_CPPFLAGS   = $(_cppflags_com)
lib@project_name@_la_CFLAGS     = $(_cflags_com)
lib@project_name@_la_CXXFLAGS   = $(_cxxflags_com)
lib@project_name@_la_LDFLAGS    = $(_ldflags_com)
lib@project_name@_la_LIBADD     = $(_ldadd_com)

