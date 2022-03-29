# ===============================================================
# 
# Release under GPLv-3.0.
# 
# @file    Makefile.am
# @brief   
# @author  gnsyxiang <gnsyxiang@163.com>
# @date    05/03 2021 10:32
# @version v0.0.1
# 
# @since    note
# @note     note
# 
#     change log:
#     NO.     Author              Date            Modified
#     00      zhenquan.qiu        05/03 2021      create the file
# 
#     last modified: 05/03 2021 10:32
# ===============================================================

include res/doxygen.am

SUBDIRS = \
          doc           \
          res           \
          include       \
          src           \
          example       \
          build-script

# ------------------
# defining variables
# ------------------
_install_files_com  =
_dist_files_com     =
include_com         =
ldadd_com           =

ACLOCAL_AMFLAGS     = -I m4
include_com         += $(ACLOCAL_AMFLAGS)

if COMPILE_LIBHY_HAL
include_com         += $(HY_HAL_INCS)
ldadd_com           += $(HY_HAL_LIBS)
endif

if COMPILE_LIBCURL
include_com         += $(CURL_INCS)
ldadd_com           += $(CURL_LIBS)
endif

if COMPILE_LIBPROTOBUF_C
include_com         += $(LIBPROTOBUF_C_INCS)
ldadd_com           += $(LIBPROTOBUF_C_LIBS)
endif

if COMPILE_LIBLOG4CPLUS
include_com         += $(LOG4CPLUS_INCS)
ldadd_com           += $(LOG4CPLUS_LIBS)
endif

if COMPILE_LIBCJSON
include_com         += $(CJSON_INCS)
ldadd_com           += $(CJSON_LIBS)
endif

export include_com
export ldadd_com 

# ---------------------
# add necessary content
# ---------------------


# ------------------
# make install files
# ------------------
_install_files_com  += \
                       LICENSE      \
                       README.md
dist_doc_DATA       = $(_install_files_com)
pkgconfig_DATA      = @project_name@.pc

# ---------------
# make dist files
# ---------------
_dist_files_com     += \
                       autogen.sh   \
                       build.sh     \
                       @project_name@.pc.in
EXTRA_DIST          = $(_dist_files_com)

DISTCLEANFILES = @project_name@.pc

