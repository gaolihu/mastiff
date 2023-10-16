#!/bin/bash
unset TOPDIR
TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"

source ${TOPDIR}/scripts/mastiff.bashrc

RTPS_LIB=$TOPDIR/.cache_build/3rd_party/fastrtps/x86_64/fast-rtps-1.5.0-1/lib
UUID_LIB=$TOPDIR/.cache_build/3rd_party/uuid/libuuid-1.0.3/x86_64//lib/
CBR_LIB=$TOPDIR/bazel-bin/cyber/proto/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$UUID_LIB:$RTPS_LIB:$CBR_LIB

cd `find . -maxdepth 1 -name "cyber"`
export CYBER_PATH=`realpath .`
cd -

bazel_bin_path="${TOPDIR}/bazel-bin"
mainboard_path="${bazel_bin_path}/cyber/mainboard"
cyber_tool_path="${bazel_bin_path}/CyberRT_tools"
recorder_path="${cyber_tool_path}/cyber_recorder"
launch_path="${cyber_tool_path}/cyber_launch"
channel_path="${cyber_tool_path}/cyber_channel"
node_path="${cyber_tool_path}/cyber_node"
service_path="${cyber_tool_path}/cyber_service"
monitor_path="${cyber_tool_path}/cyber_monitor"
visualizer_path="${bazel_bin_path}/modules/tools/visualizer"

# TODO(all): place all these in one place and pathprepend
for entry in "${mainboard_path}" \
    "${recorder_path}" "${monitor_path}"  \
    "${channel_path}" "${node_path}" \
    "${service_path}" \
    "${launch_path}" \
    "${visualizer_path}" ; do
    pathprepend "${entry}"
done

pathprepend ${bazel_bin_path}/cyber/python/internal PYTHONPATH

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

source CyberRT_tools/cyber_tools_auto_complete.bash

good "Running programs in ./bazel-bin"
