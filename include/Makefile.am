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
_no_install_headers_com =
_install_headers_com    =

install_includedir      = $(includedir)/@project_name@

# ---------------------
# add necessary content
# ---------------------
_no_install_headers_com +=

_install_headers_com    += \
                           hy_assert.h \
                           hy_bit.h \
                           hy_compile.h \
                           hy_fifo.h \
                           hy_hex.h \
                           hy_list.h \
                           hy_md5sum.h \
                           hy_mem.h \
                           hy_module.h \
                           hy_sort.h \
                           hy_string.h \
                           hy_utils.h

# ------------------
# make install files
# ------------------
noinst_HEADERS          = $(_no_install_headers_com)
install_include_HEADERS = $(_install_headers_com)
