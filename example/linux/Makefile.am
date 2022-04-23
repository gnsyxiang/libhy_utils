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

SUBDIRS = \
          ipc   \
          net   \
          timer

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
                   $(top_builddir)/src/lib@project_name@.la \
                   $(ldadd_com)

bin_PROGRAMS                        += hy_fifo_demo
hy_fifo_demo_SOURCES                = $(_sources_com) hy_fifo_demo.c
hy_fifo_demo_CPPFLAGS               = $(_cppflags_com)
hy_fifo_demo_CFLAGS                 = $(_cflags_com)
hy_fifo_demo_CXXFLAGS               = $(_cxxflags_com)
hy_fifo_demo_LDFLAGS                = $(_ldflags_com)
hy_fifo_demo_LDADD                  = $(_ldadd_com)

bin_PROGRAMS                        += hy_hash_demo
hy_hash_demo_SOURCES                = $(_sources_com) hy_hash_demo.c
hy_hash_demo_CPPFLAGS               = $(_cppflags_com)
hy_hash_demo_CFLAGS                 = $(_cflags_com)
hy_hash_demo_CXXFLAGS               = $(_cxxflags_com)
hy_hash_demo_LDFLAGS                = $(_ldflags_com)
hy_hash_demo_LDADD                  = $(_ldadd_com)

bin_PROGRAMS                        += hy_template_linux_demo
hy_template_linux_demo_SOURCES      = $(_sources_com) hy_template_linux_demo.c
hy_template_linux_demo_CPPFLAGS     = $(_cppflags_com)
hy_template_linux_demo_CFLAGS       = $(_cflags_com)
hy_template_linux_demo_CXXFLAGS     = $(_cxxflags_com)
hy_template_linux_demo_LDFLAGS      = $(_ldflags_com)
hy_template_linux_demo_LDADD        = $(_ldadd_com)

if COMPILE_LIBPROTOBUF_C
bin_PROGRAMS                        += hy_protobuf-c_demo
hy_protobuf_c_demo_SOURCES          = $(_sources_com) hy_protobuf-c_demo.c
hy_protobuf_c_demo_CPPFLAGS         = $(_cppflags_com)
hy_protobuf_c_demo_CFLAGS           = $(_cflags_com)
hy_protobuf_c_demo_CXXFLAGS         = $(_cxxflags_com)
hy_protobuf_c_demo_LDFLAGS          = $(_ldflags_com)
hy_protobuf_c_demo_LDADD            = $(_ldadd_com)

address_book.pb.c address_book.pb.h: $(abs_srcdir)/address_book.proto
	$(AM_V_GEN)@PROTOC_C@ --c_out=. --proto_path=$(abs_srcdir) address_book.proto

BUILT_SOURCES += \
                 address_book.pb.c \
                 address_book.pb.h

CLEANFILES += $(BUILT_SOURCES)
endif

bin_PROGRAMS                        += hy_thread_pool_demo
hy_thread_pool_demo_SOURCES         = $(_sources_com) hy_thread_pool_demo.c
hy_thread_pool_demo_CPPFLAGS        = $(_cppflags_com)
hy_thread_pool_demo_CFLAGS          = $(_cflags_com)
hy_thread_pool_demo_CXXFLAGS        = $(_cxxflags_com)
hy_thread_pool_demo_LDFLAGS         = $(_ldflags_com)
hy_thread_pool_demo_LDADD           = $(_ldadd_com)

bin_PROGRAMS                        += hy_timer_demo
hy_timer_demo_SOURCES               = $(_sources_com) hy_timer_demo.c
hy_timer_demo_CPPFLAGS              = $(_cppflags_com)
hy_timer_demo_CFLAGS                = $(_cflags_com)
hy_timer_demo_CXXFLAGS              = $(_cxxflags_com)
hy_timer_demo_LDFLAGS               = $(_ldflags_com)
hy_timer_demo_LDADD                 = $(_ldadd_com)

bin_PROGRAMS                        += hy_zone_demo
hy_zone_demo_SOURCES                = $(_sources_com) hy_zone_demo.c
hy_zone_demo_CPPFLAGS               = $(_cppflags_com)
hy_zone_demo_CFLAGS                 = $(_cflags_com)
hy_zone_demo_CXXFLAGS               = $(_cxxflags_com)
hy_zone_demo_LDFLAGS                = $(_ldflags_com)
hy_zone_demo_LDADD                  = $(_ldadd_com)

