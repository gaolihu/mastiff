#!/bin/bash
unset TOPDIR
TOPDIR="/data/"

alias cv='cd /data'

export CYBER_PATH="/data/cyber/"
export CYBER_DOMAIN_ID=80
export CYBER_IP=127.0.0.1

export GLOG_log_dir="${TOPDIR}/INFO.log/log"

if [ ! -d ${TOPDIR}/INFO.log/log/ ]; then
    mkdir -p ${TOPDIR}/INFO.log/log/
fi

export GLOG_alsologtostderr=1
export GLOG_colorlogtostderr=1
export GLOG_minloglevel=0

export sysmo_start=0

# for DEBUG log
#export GLOG_v=4

export VALGRIND_LIB=/data/lib/valgrind

echo -e "\n\033[31;32m Running programs in /data/ \033[0m\n"
