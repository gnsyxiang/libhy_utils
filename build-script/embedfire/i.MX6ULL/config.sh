#!/usr/bin/env bash

chip=i.MX6ULL

product=I.MX6ULL

language=cn en

host=arm-linux-gnueabihf
#cross_gcc_path=/mnt/rm-data/i.MX6ULL/gcc/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
cross_gcc_path=arm-linux-gnueabihf-

configure_param=

cppflag=-fstack-protector-all
cflag=-Wno-error=unused-value
cxxflag=
ldflag=-rdynamic
lib=

install_path=/mnt/nfs/embedfire/i.MX6ULL
