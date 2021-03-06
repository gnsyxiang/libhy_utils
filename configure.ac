#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

m4_include([m4/check_project_info.m4])

AC_INIT([PROJECT_NAME], [VERSION_NUM], [EMAIL_INFO], [PROJECT_NAME], [PROJECT_URL])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AM_SILENT_RULES([yes])
AM_PROG_AR

LT_INIT

PKG_PROG_PKG_CONFIG([0.24])
PKG_INSTALLDIR

# check m4 macro
CHECK_PROJECT_INFO
CHECK_VERSION_INFO
CHECK_DOXYGEN
# CHECK_JAVA

SELECT_VENDER
SELECT_CHIP
# SELECT_PRODUCT
# SELECT_LANGUAGE

AC_ARG_ENABLE([debug_info],
              [AS_HELP_STRING([--enable-debug_info], [enable support for output debug info])],
              [], [enable_debug_info=no])
AM_CONDITIONAL(COMPILE_DEBUG_INFO, [test "$enable_debug_info" = "yes"])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_RANLIB
AC_PROG_MKDIR_P

CHECK_LIBHY_HAL
CHECK_LIBCJSON
CHECK_LIBLOG4CPLUS
CHECK_LIBPROTOBUF_C
CHECK_LIBZ
CHECK_LIBSSL
CHECK_LIBCURL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([clock_gettime gettimeofday memset putenv select socket strstr tzset])

AC_CONFIG_FILES([Makefile
                 $project_name.pc
                 build-script/Makefile
                 build-script/arterytek/Makefile
                 build-script/arterytek/at32f4xx/Makefile
                 build-script/eeasytech/Makefile
                 build-script/eeasytech/SV823/Makefile
                 build-script/fullhan/MC6810E/Makefile
                 build-script/fullhan/Makefile
                 build-script/pc/Makefile
                 build-script/pc/pc-chip/Makefile
                 doc/Makefile
                 example/Makefile
                 example/linux/Makefile
                 example/linux/ipc/Makefile
                 example/linux/ipc/ipc_process/Makefile
                 example/linux/ipc/ipc_socket/Makefile
                 example/linux/net/Makefile
                 example/linux/timer/Makefile
                 example/mcu/Makefile
                 include/Makefile
                 include/linux/Makefile
                 include/linux/ipc/Makefile
                 include/linux/ipc/ipc_process/Makefile
                 include/linux/ipc/ipc_socket/Makefile
                 include/linux/net/Makefile
                 include/linux/timer/Makefile
                 include/mcu/Makefile
                 res/Makefile
                 src/Makefile
                 src/linux/Makefile
                 src/linux/ipc/Makefile
                 src/linux/ipc/ipc_process/Makefile
                 src/linux/ipc/ipc_socket/Makefile
                 src/linux/net/Makefile
                 src/linux/timer/Makefile
                 src/mcu/Makefile
                 ]) # for make dist
AC_OUTPUT

AC_MSG_RESULT([
               $PACKAGE $VERSION

               CC:                  ${CC}
               CPPFLAGS:            ${CPPFLAGS}
               CFLAGS:              ${CFLAGS}
               CXX:                 ${CXX}
               CXXFLAGS:            ${CXXFLAGS}
               LDFLAGS:             ${LDFLAGS}
               LIBS:                ${LIBS}

               prefix:              ${prefix}
               sysconfdir:          ${sysconfdir}
               libdir:              ${libdir}
               includedir:          ${includedir}
               pkgconfigdir:        ${pkgconfigdir}
               ])
