#!/usr/bin/env bash

chip=ubuntu

product=HY-pc

language=cn en

host=
cross_gcc_path=

configure_param=

cppflag=
cflag=
cxxflag=
ldflag=-rdynamic
lib=
debug=-ggdb -g -O0 -fstack-protector-all
release=-O2 -DNDEBUG

install_path=/mnt/nfs/pc/ubuntu

