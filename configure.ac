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
CHECK_DEBUG_RELEASE
# CHECK_JAVA

SELECT_VENDER
SELECT_CHIP
# SELECT_PRODUCT
# SELECT_LANGUAGE

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([dl], [dlopen])

CHECK_LIBCUNIT
CHECK_LIBHY_LOG
CHECK_LIBCJSON

CHECK_LIBLOG4CPLUS
CHECK_LIBPROTOBUF_C
CHECK_LIBZ
CHECK_LIBSSL
CHECK_LIBCURL
CHECK_PTHREAD

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
# AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit clock_gettime gettimeofday inet_ntoa localtime_r memset putenv select socket strerror strstr tzset])

AC_CONFIG_FILES([Makefile
                 $project_name.pc
                 build-script/Makefile
                 build-script/arterytek/Makefile
                 build-script/arterytek/at32f4xx/Makefile
                 build-script/eeasytech/Makefile
                 build-script/eeasytech/SV823/Makefile
                 build-script/esp32/Makefile
                 build-script/esp32/esp32/Makefile
                 build-script/fullhan/MC6810E/Makefile
                 build-script/fullhan/Makefile
                 build-script/nxp/Makefile
                 build-script/nxp/mx6ull/Makefile
                 build-script/pc/Makefile
                 build-script/pc/ubuntu-pc/Makefile
                 build-script/pc/windows-pc/Makefile
                 build-script/rock-chips/Makefile
                 build-script/rock-chips/rk3568/Makefile
                 build-script/stm32/Makefile
                 build-script/stm32/stm32h7xx/Makefile
                 doc/Makefile
                 example/Makefile
                 example/linux/Makefile
                 example/linux/fifo/Makefile
                 example/linux/gpio/Makefile
                 example/linux/ipc/Makefile
                 example/linux/ipc/ipc_process/Makefile
                 example/linux/ipc/ipc_socket/Makefile
                 example/linux/net/Makefile
                 example/linux/serialize/Makefile
                 example/linux/thread/Makefile
                 example/linux/time/Makefile
                 example/linux/uart/Makefile
                 example/mcu/Makefile
                 example/windows/Makefile
                 include/Makefile
                 include/linux/Makefile
                 include/linux/audio/Makefile
                 include/linux/download/Makefile
                 include/linux/fifo/Makefile
                 include/linux/gpio/Makefile
                 include/linux/ipc/Makefile
                 include/linux/ipc/ipc_process/Makefile
                 include/linux/ipc/ipc_socket/Makefile
                 include/linux/net/Makefile
                 include/linux/serialize/Makefile
                 include/linux/thread/Makefile
                 include/linux/time/Makefile
                 include/linux/uart/Makefile
                 include/mcu/Makefile
                 include/windows/Makefile
                 res/Makefile
                 res/voice/Makefile
                 res/voice/cn/Makefile
                 res/voice/en/Makefile
                 src/Makefile
                 src/linux/Makefile
                 src/linux/audio/Makefile
                 src/linux/download/Makefile
                 src/linux/fifo/Makefile
                 src/linux/gpio/Makefile
                 src/linux/ipc/Makefile
                 src/linux/ipc/ipc_process/Makefile
                 src/linux/ipc/ipc_socket/Makefile
                 src/linux/net/Makefile
                 src/linux/serialize/Makefile
                 src/linux/thread/Makefile
                 src/linux/time/Makefile
                 src/linux/uart/Makefile
                 src/mcu/Makefile
                 src/windows/Makefile
                 test/Makefile])
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
