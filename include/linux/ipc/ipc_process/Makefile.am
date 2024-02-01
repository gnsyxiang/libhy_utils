# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    02/03 2022 16:48
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        02/03 2022      create the file
# 
#     last modified: 02/03 2022 16:48
# ===============================================================

# ------------------
# defining variables
# ------------------
_no_install_headers_com =
_install_headers_com    =

install_includedir      = $(includedir)/$(project_name)

# ---------------------
# add necessary content
# ---------------------
_no_install_headers_com += \
                           ipc_link.h           \
                           ipc_link_manager.h   \
                           ipc_process_client.h \
                           ipc_process_server.h

_install_headers_com    += \
                           hy_ipc_process.h     \
                           hy_ipc_process_id.h

# ------------------
# make install files
# ------------------
noinst_HEADERS          = $(_no_install_headers_com)
install_include_HEADERS = $(_install_headers_com)

