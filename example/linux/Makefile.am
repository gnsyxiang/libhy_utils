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

SUBDIRS = ipc

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

_ldadd_com      += $(top_builddir)/src/lib@project_name@.la

if COMPILE_LIBHY_HAL
_include_com    += $(HY_HAL_INCS)
_ldadd_com      += $(HY_HAL_LIBS)
endif

if COMPILE_LIBCJSON
_include_com    += $(CJSON_INCS)
_ldadd_com      += $(CJSON_LIBS)
endif

if COMPILE_LIBLOG4CPLUS
_include_com    += $(LOG4CPLUS_INCS)
_ldadd_com      += $(LOG4CPLUS_LIBS)
endif

bin_PROGRAMS                += hy_fifo_test
hy_fifo_test_SOURCES        = $(_sources_com) hy_fifo_test.c
hy_fifo_test_CPPFLAGS       = $(_cppflags_com)
hy_fifo_test_CFLAGS         = $(_cflags_com)
hy_fifo_test_CXXFLAGS       = $(_cxxflags_com)
hy_fifo_test_LDFLAGS        = $(_ldflags_com)
hy_fifo_test_LDADD          = $(_ldadd_com)

bin_PROGRAMS                += hy_hash_test
hy_hash_test_SOURCES        = $(_sources_com) hy_hash_test.c
hy_hash_test_CPPFLAGS       = $(_cppflags_com)
hy_hash_test_CFLAGS         = $(_cflags_com)
hy_hash_test_CXXFLAGS       = $(_cxxflags_com)
hy_hash_test_LDFLAGS        = $(_ldflags_com)
hy_hash_test_LDADD          = $(_ldadd_com)

bin_PROGRAMS                += hy_template_test
hy_template_test_SOURCES    = $(_sources_com) hy_template_test.c
hy_template_test_CPPFLAGS   = $(_cppflags_com)
hy_template_test_CFLAGS     = $(_cflags_com)
hy_template_test_CXXFLAGS   = $(_cxxflags_com)
hy_template_test_LDFLAGS    = $(_ldflags_com)
hy_template_test_LDADD      = $(_ldadd_com)

bin_PROGRAMS                += hy_net_wired_test
hy_net_wired_test_SOURCES   = $(_sources_com) hy_net_wired_test.c
hy_net_wired_test_CPPFLAGS  = $(_cppflags_com)
hy_net_wired_test_CFLAGS    = $(_cflags_com)
hy_net_wired_test_CXXFLAGS  = $(_cxxflags_com)
hy_net_wired_test_LDFLAGS   = $(_ldflags_com)
hy_net_wired_test_LDADD     = $(_ldadd_com)

if COMPILE_LIBPROTOBUF_C
_include_com    += $(LIBPROTOBUF_C_INCS)
_ldadd_com      += $(LIBPROTOBUF_C_LIBS)

bin_PROGRAMS                += hy_protobuf-c_test
hy_protobuf_c_test_SOURCES  = $(_sources_com) hy_protobuf-c_test.c
hy_protobuf_c_test_CPPFLAGS = $(_cppflags_com)
hy_protobuf_c_test_CFLAGS   = $(_cflags_com)
hy_protobuf_c_test_CXXFLAGS = $(_cxxflags_com)
hy_protobuf_c_test_LDFLAGS  = $(_ldflags_com)
hy_protobuf_c_test_LDADD    = $(_ldadd_com)

address_book.pb.c address_book.pb.h: $(abs_srcdir)/address_book.proto
	$(AM_V_GEN)@PROTOC_C@ --c_out=. --proto_path=$(abs_srcdir) address_book.proto

BUILT_SOURCES += \
                 address_book.pb.c \
                 address_book.pb.h

CLEANFILES += $(BUILT_SOURCES)
endif

bin_PROGRAMS                    += hy_thread_pool_test
hy_thread_pool_test_SOURCES     = $(_sources_com) hy_thread_pool_test.c
hy_thread_pool_test_CPPFLAGS    = $(_cppflags_com)
hy_thread_pool_test_CFLAGS      = $(_cflags_com)
hy_thread_pool_test_CXXFLAGS    = $(_cxxflags_com)
hy_thread_pool_test_LDFLAGS     = $(_ldflags_com)
hy_thread_pool_test_LDADD       = $(_ldadd_com)

bin_PROGRAMS                    += hy_timer_test
hy_timer_test_SOURCES           = $(_sources_com) hy_timer_test.c
hy_timer_test_CPPFLAGS          = $(_cppflags_com)
hy_timer_test_CFLAGS            = $(_cflags_com)
hy_timer_test_CXXFLAGS          = $(_cxxflags_com)
hy_timer_test_LDFLAGS           = $(_ldflags_com)
hy_timer_test_LDADD             = $(_ldadd_com)

