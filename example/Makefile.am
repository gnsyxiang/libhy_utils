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

_include_com    += \
				   -I$(top_srcdir)/include 			\
				   -I$(top_srcdir)/include/net 		\
				   -I$(top_srcdir)/include/thread

_sources_com    +=

_cppflags_com   += $(_no_error_com) $(_include_com) $(_define_com)

_cflags_com     +=

_cxxflags_com   +=

_ldflags_com    +=

_ldadd_com      += $(top_builddir)/src/lib@project_name@.la -lpthread

if COMPILE_LIBHY_LOG
_ldflags_com    += $(HY_LOG_LIBS)
endif

bin_PROGRAMS    				+= hy_can_demo
hy_can_demo_SOURCES    			= $(_sources_com) hy_can_demo.c
hy_can_demo_CPPFLAGS   			= $(_cppflags_com)
hy_can_demo_CFLAGS     			= $(_cflags_com)
hy_can_demo_CXXFLAGS   			= $(_cxxflags_com)
hy_can_demo_LDFLAGS    			= $(_ldflags_com)
hy_can_demo_LDADD      			= $(_ldadd_com)

bin_PROGRAMS    				+= hy_template_demo
hy_template_demo_SOURCES    	= $(_sources_com) hy_template_demo.c
hy_template_demo_CPPFLAGS   	= $(_cppflags_com)
hy_template_demo_CFLAGS     	= $(_cflags_com)
hy_template_demo_CXXFLAGS   	= $(_cxxflags_com)
hy_template_demo_LDFLAGS    	= $(_ldflags_com)
hy_template_demo_LDADD      	= $(_ldadd_com)

bin_PROGRAMS    				+= hy_dynamic_array_demo
hy_dynamic_array_demo_SOURCES  	= $(_sources_com) hy_dynamic_array_demo.c
hy_dynamic_array_demo_CPPFLAGS 	= $(_cppflags_com)
hy_dynamic_array_demo_CFLAGS   	= $(_cflags_com)
hy_dynamic_array_demo_CXXFLAGS 	= $(_cxxflags_com)
hy_dynamic_array_demo_LDFLAGS  	= $(_ldflags_com)
hy_dynamic_array_demo_LDADD    	= $(_ldadd_com)

bin_PROGRAMS    				+= hy_epoll_demo
hy_epoll_demo_SOURCES  			= $(_sources_com) hy_epoll_demo.c
hy_epoll_demo_CPPFLAGS 			= $(_cppflags_com)
hy_epoll_demo_CFLAGS   			= $(_cflags_com)
hy_epoll_demo_CXXFLAGS 			= $(_cxxflags_com)
hy_epoll_demo_LDFLAGS  			= $(_ldflags_com)
hy_epoll_demo_LDADD    			= $(_ldadd_com)

bin_PROGRAMS    				+= hy_fifo_demo
hy_fifo_demo_SOURCES    		= $(_sources_com) hy_fifo_demo.c
hy_fifo_demo_CPPFLAGS   		= $(_cppflags_com)
hy_fifo_demo_CFLAGS     		= $(_cflags_com)
hy_fifo_demo_CXXFLAGS   		= $(_cxxflags_com)
hy_fifo_demo_LDFLAGS    		= $(_ldflags_com)
hy_fifo_demo_LDADD      		= $(_ldadd_com)

