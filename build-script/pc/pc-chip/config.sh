#!/usr/bin/env bash

chip=pc-chip

product=HY-pc

language=cn en

host=
cross_gcc_path=

configure_param=--enable-cjson --enable-event --enable-libprotobuf_c

cppflag=-fstack-protector-all -Wno-error=stringop-truncation -Wno-error=format-truncation= -Wno-error=format-overflow=
cflag=
cxxflag=
ldflag=-rdynamic
lib=
debug=-g -O0
release=-O2 -DNDEBUG

install_path=/mnt/nfs/pc/pc-chip

