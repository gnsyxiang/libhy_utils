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

SUBDIRS = \
		  ipc \
		  net \
		  thread \
		  timer

if COMPILE_LIBCJSON
SUBDIRS += serialize
endif

if COMPILE_LIBCURL
SUBDIRS += download
endif

# ------------------
# defining variables
# ------------------
_no_install_headers_com =
_install_headers_com    =

install_includedir      = $(includedir)/@project_name@

# ---------------------
# add necessary content
# ---------------------
_no_install_headers_com +=

_install_headers_com    += \
						   hy_gpio_interrupt.h \
						   hy_pipe.h \
						   hy_hash.h \
						   hy_math.h \
						   hy_video.h \
						   hy_audio.h \
						   hy_sort.h \
						   hy_md5sum.h \
						   hy_bit.h \
						   hy_queue.h \
						   hy_package_list.h \
						   hy_can.h \
						   hy_gpio.h \
						   hy_file.h \
						   hy_list.h \
						   hy_fifo.h \
						   hy_misc.h \
						   hy_compile.h \
						   hy_utils.h \
						   hy_hex.h \
						   hy_time.h \
						   hy_string.h \
						   hy_signal.h \
						   hy_module.h \
						   hy_barrier.h \
						   hy_dynamic_array.h \
						   hy_mem.h \
						   hy_assert.h \
						   hy_type.h

# ------------------
# make install files
# ------------------
noinst_HEADERS          = $(_no_install_headers_com)
install_include_HEADERS = $(_install_headers_com)

