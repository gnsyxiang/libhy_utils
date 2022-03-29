#!/usr/bin/env bash

chip=pc-chip

product=HY-pc

language=cn en

gcc_version=x86_64-linux-gnu
host=
cross_gcc_path=

configure_param=--enable-cjson --enable-libz --enable-ssl --enable-curl

cppflag=-fstack-protector-all
cflag=
cxxflag=
ldflag=-rdynamic
lib=

install_path=/opt/data/nfs/install/pc/pc-chip

