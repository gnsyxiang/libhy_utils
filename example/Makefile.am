# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    17/08 2023 10:03
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        17/08 2023      create the file
# 
#     last modified: 17/08 2023 10:03
# ===============================================================

SUBDIRS = $(run_os)

if COMPILE_LIBCJSON
SUBDIRS += serialize
endif

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
BUILT_SOURCES   =
CLEANFILES      =

# ---------------------
# add necessary content
# ---------------------
_no_error_com   +=

_define_com     +=

_include_com    += \
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

if COMPILE_LIBPROTOBUF_C
address_book.pb-c.c: $(abs_srcdir)/address_book.proto
	$(AM_V_GEN)@PROTOC_C@ --c_out=. --proto_path=$(abs_srcdir) address_book.proto

BUILT_SOURCES += \
                 address_book.pb-c.c

CLEANFILES += $(BUILT_SOURCES)

bin_PROGRAMS                += hy_protobuf-c_demo
hy_protobuf_c_demo_SOURCES  = $(_sources_com) hy_protobuf-c_demo.c address_book.pb-c.c
hy_protobuf_c_demo_CPPFLAGS = $(_cppflags_com)
hy_protobuf_c_demo_CFLAGS   = $(_cflags_com)
hy_protobuf_c_demo_CXXFLAGS = $(_cxxflags_com)
hy_protobuf_c_demo_LDFLAGS  = $(_ldflags_com)
hy_protobuf_c_demo_LDADD    = $(_ldadd_com)
endif
