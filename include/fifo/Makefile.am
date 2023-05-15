# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    15/05 2023 10:43
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        15/05 2023      create the file
# 
#     last modified: 15/05 2023 10:43
# ===============================================================

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
                           hy_fifo_lock.h \
                           hy_fifo.h

# ------------------
# make install files
# ------------------
noinst_HEADERS          = $(_no_install_headers_com)
install_include_HEADERS = $(_install_headers_com)

