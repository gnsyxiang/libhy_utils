#!/usr/bin/env bash

chip=ubuntu-pc

product=HY-pc

language=cn en

host=
cross_gcc_path=

configure_param=

cppflag=-fstack-protector-all
cflag=
cxxflag=
ldflag=-rdynamic
lib=
debug=-g -O0
release=-O2 -DNDEBUG

install_path=/mnt/nfs/pc/ubuntu-pc

