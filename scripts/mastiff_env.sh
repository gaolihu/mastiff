#!/bin/bash

# Environment Setting Up for Mastiff Project

#set -e

TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
source "${TOPDIR}/scripts/mastiff.bashrc"

# Alias for convenience
alias croot='cd $TOPDIR'
alias cr='cd $TOPDIR'
alias cbr='cd $TOPDIR/cyber'

# Project build cache locally
BUILD_DIR=.cache_build
BUILD_PATH=$TOPDIR/$BUILD_DIR

# Use bazel version
BZL_VERSION=5.4.1

# Remote http server site
#REMOTE_IP=xxx.xxx.xxx.xxx
REMOTE_IP=10.10.3.27
REMOTE_HTTP_SERVER=http://$REMOTE_IP/third_party

# Platforms setting
PLAT_X64=x86_64
PLAT_ARM64=aarch64
PLAT_ARM32=arm

# Install cross compile toolchains locally
ARM_AARCH_6_4_TOOL_PATH=$BUILD_PATH/tool_chains
AARCH_9_3_TOOL_PATH=$BUILD_PATH/tool_chains
AARCH_9_4_TOOL_PATH=$BUILD_PATH/tool_chains
AARCH_10_3_TOOL_PATH=$BUILD_PATH/tool_chains
AARCH_11_3_TOOL_PATH=$BUILD_PATH/tool_chains

# Cross compile toolchains name
ARM_TOOL_CHAIN="arm-linux-gnueabihf-"
AARCH64_TOOL_CHAIN="aarch64-linux-gnu-"
AARCH93_TOOL_CHAIN="aarch64-linux-"
AARCH94_TOOL_CHAIN="aarch64-linux-"
AARCH103_TOOL_CHAIN="aarch64-rockchip1031-linux-gnu-"
AARCH113_TOOL_CHAIN="aarch64-buildroot-linux-gnu-"
X86_TOOL_CHAIN=""

ARM_TOOL_CHAIN_DIR=""
AARCH_TOOL_CHAIN_DIR=""
AARCH93_TOOL_CHAIN_DIR=""
AARCH94_TOOL_CHAIN_DIR=""
AARCH103_TOOL_CHAIN_DIR=""
AARCH113_TOOL_CHAIN_DIR=""
X86_TOOL_CHAIN_DIR=""

# Third party libraries dependences
EXTERNAL_DIR=external
EXTERNAL_BUILD_PATH=$BUILD_PATH/$EXTERNAL_DIR
if [ ! -d $EXTERNAL_BUILD_PATH ]; then
    mkdir -p $EXTERNAL_BUILD_PATH
fi

if [ ! -L $TOPDIR/mstf_build.sh ]; then
    ln -fs $TOPDIR/scripts/mastiff_build.sh mstf_build.sh
fi

if [ ! -L $TOPDIR/mstf_clean.sh ]; then
    ln -fs $TOPDIR/scripts/mastiff_clean.sh mstf_clean.sh
fi

PKG_R_PATH=$BUILD_DIR/$EXTERNAL_DIR
export PKG_R_PATH

# Cyber core dependence fastrtps
EX_WORKSPACK="workspace.bzl"
FASTRTPS_PKG_NAME=fastrtps
FASTCDR_PKG_NAME=fastcdr
FASTRTPS_PKG_VERSION="fast-rtps-1.5.0-1"
FASTRTPS_PKGX64_VERBOSE="fast-rtps-1.5.0-1.prebuilt.x86_64.tar.gz"
FASTRTPS_PKGARM64_VERBOSE="fast-rtps-1.5.0-1.prebuilt.aarch64.tar.gz"
FASTRTPS_PKG_PATH=$EXTERNAL_BUILD_PATH/$FASTRTPS_PKG_NAME
FASTRTPS_INSTALL_X64_PATH=$FASTRTPS_PKG_PATH/$PLAT_X64/$FASTRTPS_PKG_VERSION
FASTRTPS_INSTALL_ARM64_PATH=$FASTRTPS_PKG_PATH/$PLAT_ARM64/$FASTRTPS_PKG_VERSION
FASTRTPS_INSTALL_ARM_PATH=$FASTRTPS_PKG_PATH/$PLAT_ARM32/$FASTRTPS_PKG_VERSION

FASTRTPS_LIB_X64="$FASTRTPS_INSTALL_X64_PATH/lib"
FASTRTPS_LIB_ARM64="$FASTRTPS_INSTALL_ARM64_PATH/lib"
FASTRTPS_LIB_ARM32="$FASTRTPS_INSTALL_ARM_PATH/lib"

UUID_PKG_NAME=uuid
UUID_PKG_VERSION="libuuid-1.0.3"
UUID_PKG_VERBOSE="libuuid-1.0.3.tar.gz"
UUID_PKG_PATH=$EXTERNAL_BUILD_PATH/$UUID_PKG_NAME
UUID_INSTALL_X64_PATH=$UUID_PKG_PATH/$UUID_PKG_VERSION/$PLAT_X64
UUID_INSTALL_ARM64_PATH=$UUID_PKG_PATH/$UUID_PKG_VERSION/$PLAT_ARM64
UUID_INSTALL_ARM_PATH=$UUID_PKG_PATH/$UUID_PKG_VERSION/$PLAT_ARM32

UUID_LIB_X64="$UUID_INSTALL_X64_PATH/lib"
UUID_LIB_ARM64="$UUID_INSTALL_ARM64_PATH/lib"
UUID_LIB_ARM32="$UUID_INSTALL_ARM_PATH/lib"

ROS_PATH=$BUILD_PATH/ros/melodic/

function check_remote_server() {
    if [[ "$REMOTE_IP" = "xxx.xxx.xxx.xxx" ]]; then
        REMOTE_IP=$(echo "`ifconfig`" | grep -oE 'inet (addr:)?([0-9]*\.){3}[0-9]*')
        REMOTE_IP=`echo "$REMOTE_IP" | sed -e '/127.0.0.1/d;/172.17.0.1/d'`  # localhost / docker ip
        REMOTE_IP=`echo "$REMOTE_IP" | sed -e 's@inet @@'`
        REMOTE_HTTP_SERVER=http://$REMOTE_IP/third_party
    fi

    #ping $REMOTE_IP -c 3

    if [ $? -eq 0 ]; then
        wget -q $REMOTE_HTTP_SERVER/TEST_FILE_SEVER
        if [ $? -eq 0 ]; then
            rm TEST_FILE_SEVER
            good "server OK: $REMOTE_HTTP_SERVER"
            return 0
        else
            error "server($REMOTE_IP) is not available!"
            return -1
        fi
    else
        error "can't get acess to $REMOTE_IP!"
        return -1
    fi

}

function install_toolchain() {
    cd $TOPDIR/

    info "check & install toolchains for Mastiff!"

    if [ ! -e $ARM_AARCH_6_4_TOOL_PATH/gcc-6.4.1-x86_64-arm-aarch64-linux-gnu.tgz ]; then
        wget -nv $REMOTE_HTTP_SERVER/tool_chains/gcc-6.4.1-x86_64-arm-aarch64-linux-gnu.tgz -P $ARM_AARCH_6_4_TOOL_PATH
        if [ $? -ne 0 ]; then
            error "can not get gcc-6.4.1-x86_64-arm-aarch64-linux-gnu.tgz!"
            return -1
        fi
        cd $ARM_AARCH_6_4_TOOL_PATH
        tar xzf gcc-6.4.1-x86_64-arm-aarch64-linux-gnu.tgz
        [[ $? -eq 0 ]] && \
            good "install cross toolchains gcc 6.4.1 OK!" || \
            error "install cross toolchains gcc 6.4.1 NG!"
    fi
    ARM_AARCH_6_4_TOOL_PATH=$ARM_AARCH_6_4_TOOL_PATH/gcc-6.4.1-x86_64-arm-aarch64-linux-gnu/

    if [ ! -e $AARCH_9_3_TOOL_PATH/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu.tgz ]; then
        wget -nv $REMOTE_HTTP_SERVER/tool_chains/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu.tgz -P $AARCH_9_3_TOOL_PATH
        if [ $? -ne 0 ]; then
            error "can not get gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu.tgz"
            return -1
        fi
        cd $AARCH_9_3_TOOL_PATH
        tar xzf gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu.tgz
        [[ $? -eq 0 ]] && \
            good "install cross toolchains gcc 9.3 OK!" || \
            error "install cross toolchains gcc 9.3 NG!"
    fi
    AARCH_9_3_TOOL_PATH=$AARCH_9_3_TOOL_PATH/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/

    if [ ! -e $AARCH_9_4_TOOL_PATH/gcc-buildroot-9.4.0-2021.01-x86_64_aarch64-rockchip-linux-gnu.tgz ]; then
        wget -nv $REMOTE_HTTP_SERVER/tool_chains/gcc-buildroot-9.4.0-2021.01-x86_64_aarch64-rockchip-linux-gnu.tgz -P $AARCH_9_4_TOOL_PATH
        if [ $? -ne 0 ]; then
            error "can not get gcc-buildroot-9.4.0-2021.01-x86_64_aarch64-rockchip-linux-gnu.tgz"
            return -1
        fi
        cd $AARCH_9_4_TOOL_PATH
        tar xzf gcc-buildroot-9.4.0-2021.01-x86_64_aarch64-rockchip-linux-gnu.tgz
        [[ $? -eq 0 ]] && \
            good "install cross toolchains gcc 9.4 OK!" || \
            error "install cross toolchains gcc 9.4 NG!"
    fi
    AARCH_9_4_TOOL_PATH=$AARCH_9_4_TOOL_PATH/gcc-buildroot-9.4.0-2021.01-x86_64_aarch64-rockchip-linux-gnu/

    if [ ! -e $AARCH_10_3_TOOL_PATH/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tgz ]; then
        wget -nv $REMOTE_HTTP_SERVER/tool_chains/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tgz -P $AARCH_10_3_TOOL_PATH
        if [ $? -ne 0 ]; then
            error "can not get gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tgz"
            return -1
        fi
        cd $AARCH_10_3_TOOL_PATH
        tar xzf gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tgz
        [[ $? -eq 0 ]] && \
            good "install cross toolchains gcc 10.3 OK!" || \
            error "install cross toolchains gcc 10.3 NG!"
    fi
    AARCH_10_3_TOOL_PATH=$AARCH_10_3_TOOL_PATH/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/

    if [ ! -e $AARCH_11_3_TOOL_PATH/gcc-buildroot-11.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu.tgz ]; then
        wget -nv $REMOTE_HTTP_SERVER/tool_chains/gcc-buildroot-11.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu.tgz -P $AARCH_11_3_TOOL_PATH
        if [ $? -ne 0 ]; then
            error "can not get gcc-buildroot-11.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu.tgz"
            return -1
        fi
        cd $AARCH_11_3_TOOL_PATH
        tar xzf gcc-buildroot-11.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu.tgz
        [[ $? -eq 0 ]] && \
            good "install cross toolchains gcc 11.3 OK!" || \
            error "install cross toolchains gcc 11.3 NG!"
    fi
    AARCH_11_3_TOOL_PATH=$AARCH_11_3_TOOL_PATH/gcc-buildroot-11.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/

    arm64_tool=`ls $ARM_AARCH_6_4_TOOL_PATH/$PLAT_ARM32/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/${ARM_TOOL_CHAIN}gcc`
    aarch64_tool=`ls $ARM_AARCH_6_4_TOOL_PATH/$PLAT_ARM64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/${AARCH64_TOOL_CHAIN}gcc`
    aarch93_tool=`ls $AARCH_9_3_TOOL_PATH/bin/${AARCH93_TOOL_CHAIN}gcc`
    aarch94_tool=`ls $AARCH_9_4_TOOL_PATH/bin/${AARCH94_TOOL_CHAIN}gcc`
    aarch103_tool=`ls $AARCH_10_3_TOOL_PATH/bin/${AARCH103_TOOL_CHAIN}gcc`
    aarch113_tool=`ls $AARCH_11_3_TOOL_PATH/bin/${AARCH113_TOOL_CHAIN}gcc`
    x86_tool=`which ${X86_TOOL_CHAIN}gcc`

    ARM_TOOL_CHAIN_DIR=`dirname $arm64_tool`
    AARCH_TOOL_CHAIN_DIR=`dirname $aarch64_tool`
    AARCH93_TOOL_CHAIN_DIR=`dirname $aarch93_tool`
    AARCH94_TOOL_CHAIN_DIR=`dirname $aarch94_tool`
    AARCH103_TOOL_CHAIN_DIR=`dirname $aarch103_tool`
    AARCH113_TOOL_CHAIN_DIR=`dirname $aarch113_tool`
    X86_TOOL_CHAIN_DIR=`dirname $x86_tool`

    #info "arm64_tool: $arm64_tool"
    #info "aarch64_tool: $aarch64_tool"
    #info "aarch93_tool: $aarch93_tool"
    #info "aarch94_tool: $aarch94_tool"
    #info "aarch103_tool: $aarch103_tool"
    #info "aarch113_tool: $aarch113_tool"

    #info "ARM_TOOL_CHAIN_DIR: $ARM_TOOL_CHAIN_DIR"
    #info "AARCH_TOOL_CHAIN_DIR: $AARCH_TOOL_CHAIN_DIR"
    #info "AARCH93_TOOL_CHAIN_DIR: $AARCH93_TOOL_CHAIN_DIR"
    #info "AARCH94_TOOL_CHAIN_DIR: $AARCH94_TOOL_CHAIN_DIR"
    #info "AARCH103_TOOL_CHAIN_DIR: $AARCH103_TOOL_CHAIN_DIR"
    #info "AARCH113_TOOL_CHAIN_DIR: $AARCH113_TOOL_CHAIN_DIR"

    export ARM_TOOL_CHAIN_DIR
    export AARCH_TOOL_CHAIN_DIR
    export AARCH93_TOOL_CHAIN_DIR
    export AARCH94_TOOL_CHAIN_DIR
    export AARCH103_TOOL_CHAIN_DIR
    export AARCH113_TOOL_CHAIN_DIR

    cd $TOPDIR/$EXTERNAL_DIR/toolchain/

    cp mastiff_build_conf.bzl.tpl mastiff_build_conf.bzl

    sed -i "s#@PATH_TO_GCC_ARM@#$ARM_TOOL_CHAIN_DIR#g" mastiff_build_conf.bzl
    sed -i "s#@GCC_ARCH_ARM@#$ARM_TOOL_CHAIN#g" mastiff_build_conf.bzl

    sed -i "s#@PATH_TO_GCC_AARCH93@#$AARCH93_TOOL_CHAIN_DIR#g" mastiff_build_conf.bzl
    sed -i "s#@GCC_ARCH_AARCH93@#$AARCH93_TOOL_CHAIN#g" mastiff_build_conf.bzl

    sed -i "s#@PATH_TO_GCC_AARCH94@#$AARCH94_TOOL_CHAIN_DIR#g" mastiff_build_conf.bzl
    sed -i "s#@GCC_ARCH_AARCH94@#$AARCH94_TOOL_CHAIN#g" mastiff_build_conf.bzl

    sed -i "s#@PATH_TO_GCC_AARCH103@#$AARCH103_TOOL_CHAIN_DIR#g" mastiff_build_conf.bzl
    sed -i "s#@GCC_ARCH_AARCH103@#$AARCH103_TOOL_CHAIN#g" mastiff_build_conf.bzl

    sed -i "s#@PATH_TO_GCC_AARCH113@#$AARCH113_TOOL_CHAIN_DIR#g" mastiff_build_conf.bzl
    sed -i "s#@GCC_ARCH_AARCH113@#$AARCH113_TOOL_CHAIN#g" mastiff_build_conf.bzl

    sed -i "s#@PATH_TO_GCC_AARCH@#$AARCH_TOOL_CHAIN_DIR#g" mastiff_build_conf.bzl
    sed -i "s#@GCC_ARCH_AARCH@#$AARCH64_TOOL_CHAIN#g" mastiff_build_conf.bzl

    sed -i "s#@PATH_TO_GCC_X86@#$X86_TOOL_CHAIN_DIR#g" mastiff_build_conf.bzl
    sed -i "s#@GCC_ARCH_X86@#$X86_TOOL_CHAIN#g" mastiff_build_conf.bzl

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BUILD_PATH/tool_chains/gcc-buildroot-9.3.0-2020.03-x86_64_aarch64-rockchip-linux-gnu/lib

    info "check & install cross toolchains OK!"

    cd $TOPDIR
}

function install_bazel() {
    cd $TOPDIR

    if [ ! -d $BUILD_PATH/bazel_tool ]; then
        mkdir -p $BUILD_PATH/bazel_tool
    fi

    if [ ! -f $BUILD_PATH/bazel_tool/VERSION ]; then
        info "install bazel version: $BZL_VERSION"

        #download bazel script for linux
        #https://github.com/bazelbuild/bazel/releases/download/6.1.2/bazel-6.1.2-installer-linux-x86_64.sh
        #https://github.com/bazelbuild/bazel/releases/download/6.2.1/bazel-6.2.1-installer-linux-x86_64.sh
        #https://github.com/bazelbuild/bazel/releases/download/6.2.0/bazel-6.2.0-installer-linux-x86_64.sh
        #https://github.com/bazelbuild/bazel/releases/download/5.4.1/bazel-5.4.1-installer-linux-x86_64.sh

        if [ -e $TOPDIR/bzel/install/bazel-$BZL_VERSION-installer-linux-x86_64.sh ]; then
            info "bazel install locally"
            chmod a+x $TOPDIR/bzel/install/bazel-$BZL_VERSION-installer-linux-x86_64.sh
            $TOPDIR/bzel/install/bazel-$BZL_VERSION-installer-linux-x86_64.sh \
                --prefix=$BUILD_PATH/bazel_tool/ > /dev/null
            [[ $? -eq 0 ]] && good "install bazel locally OK!" || \
                {error "install bazel locally NG" return -1}
        else
            info "download bazel & install"
            wget -nv $REMOTE_HTTP_SERVER/bazel/bazel-$BZL_VERSION-installer-linux-x86_64.sh \
                -P $TOPDIR/bzel/install/
            if [ $? -ne 0 ]; then
                error "can not get bazel-$BZL_VERSION-installer-linux-x86_64.sh"
                return -1
            fi
            chmod a+x $TOPDIR/bzel/install/bazel-$BZL_VERSION-installer-linux-x86_64.sh
            $TOPDIR/bzel/install/bazel-$BZL_VERSION-installer-linux-x86_64.sh \
                --prefix=$BUILD_PATH/bazel_tool/ > /dev/null
            [[ $? -eq 0 ]] && good "install bazel OK!" || \
                {error "install bazel NG" return -1}
        fi

        export PATH=$PATH:$BUILD_PATH/bazel_tool/bin/
        source $BUILD_PATH/bazel_tool//lib/bazel/bin/bazel-complete.bash
        echo $BZL_VERSION > $BUILD_PATH/bazel_tool/VERSION
    else
        info "bazel has been installed, skip"
        info " previous version: `cat $BUILD_PATH/bazel_tool/VERSION`"

        export PATH=$PATH:$BUILD_PATH/bazel_tool/bin/
        if [ "$?" = "1" ]; then
            export PATH=$PATH:$BUILD_PATH/bazel_tool/bin/
        fi
    fi
}

function install_fastrtps() {
    info "check & install fast rtps-cdr library"
    cd $TOPDIR

    if [ ! -d $FASTRTPS_PKG_PATH ]; then
        info "copy fast rtps & cdr library"
        cp -rf $TOPDIR/$EXTERNAL_DIR/$FASTRTPS_PKG_NAME $EXTERNAL_BUILD_PATH

        cp $TOPDIR/$EXTERNAL_DIR/$FASTRTPS_PKG_NAME/$FASTRTPS_PKG_NAME.BUILD $FASTRTPS_PKG_PATH/_$FASTRTPS_PKG_NAME.BUILD_$PLAT_X64
        cp $TOPDIR/$EXTERNAL_DIR/$FASTRTPS_PKG_NAME/$FASTRTPS_PKG_NAME.BUILD $FASTRTPS_PKG_PATH/_$FASTRTPS_PKG_NAME.BUILD_$PLAT_ARM64
        cp $TOPDIR/$EXTERNAL_DIR/$FASTRTPS_PKG_NAME/$FASTRTPS_PKG_NAME.BUILD $FASTRTPS_PKG_PATH/_$FASTRTPS_PKG_NAME.BUILD_$PLAT_ARM32
        cp $TOPDIR/$EXTERNAL_DIR/$FASTRTPS_PKG_NAME/$FASTCDR_PKG_NAME.BUILD $FASTRTPS_PKG_PATH/_$FASTCDR_PKG_NAME.BUILD_$PLAT_X64
        cp $TOPDIR/$EXTERNAL_DIR/$FASTRTPS_PKG_NAME/$FASTCDR_PKG_NAME.BUILD $FASTRTPS_PKG_PATH/_$FASTCDR_PKG_NAME.BUILD_$PLAT_ARM64
        cp $TOPDIR/$EXTERNAL_DIR/$FASTRTPS_PKG_NAME/$FASTCDR_PKG_NAME.BUILD $FASTRTPS_PKG_PATH/_$FASTCDR_PKG_NAME.BUILD_$PLAT_ARM32

        cp $TOPDIR/$EXTERNAL_DIR/$FASTRTPS_PKG_NAME/$EX_WORKSPACK $FASTRTPS_PKG_PATH/_${EX_WORKSPACK}_$PLAT_X64
        cp $TOPDIR/$EXTERNAL_DIR/$FASTRTPS_PKG_NAME/$EX_WORKSPACK $FASTRTPS_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM64
        cp $TOPDIR/$EXTERNAL_DIR/$FASTRTPS_PKG_NAME/$EX_WORKSPACK $FASTRTPS_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM32

        rm $FASTRTPS_PKG_PATH/$FASTRTPS_PKG_NAME.BUILD
        rm $FASTRTPS_PKG_PATH/$FASTCDR_PKG_NAME.BUILD
        rm $FASTRTPS_PKG_PATH/$EX_WORKSPACK

        cp $TOPDIR/$EXTERNAL_DIR/$FASTRTPS_PKG_NAME/$EX_WORKSPACK $FASTRTPS_PKG_PATH/${EX_WORKSPACK}
    fi

    if [ ! -d $FASTRTPS_INSTALL_X64_PATH ] ||
        [ ! -d $FASTRTPS_INSTALL_AARCH_PATH ]; then
        info "install fastrtps library"

        #make buildout dir
        mkdir -p $FASTRTPS_PKG_PATH/$PLAT_X64
        mkdir -p $FASTRTPS_PKG_PATH/$PLAT_ARM32
        mkdir -p $FASTRTPS_PKG_PATH/$PLAT_ARM64

        #download & decompress
        if [ ! -d $FASTRTPS_INSTALL_X64_PATH ]; then
            wget -nv $REMOTE_HTTP_SERVER/$FASTRTPS_PKG_NAME/$FASTRTPS_PKGX64_VERBOSE -P $FASTRTPS_PKG_PATH
            if [ $? -ne 0 ]; then
                error "can not get $FASTRTPS_PKGX64_VERBOSE"
                return -1
            fi
            wget -nv $REMOTE_HTTP_SERVER/$FASTRTPS_PKG_NAME/$FASTRTPS_PKGARM64_VERBOSE -P $FASTRTPS_PKG_PATH
            if [ $? -ne 0 ]; then
                error "can not get $FASTRTPS_PKGARM64_VERBOSE"
                return -1
            fi
            tar -xzf $FASTRTPS_PKG_PATH/$FASTRTPS_PKGX64_VERBOSE -C $FASTRTPS_PKG_PATH/$PLAT_X64
            tar -xzf $FASTRTPS_PKG_PATH/$FASTRTPS_PKGARM64_VERBOSE -C $FASTRTPS_PKG_PATH/$PLAT_ARM64
        fi

        #BUILD script
        sed -i "s#@FASTRTPS_LIB@#$FASTRTPS_LIB_X64#g" $FASTRTPS_PKG_PATH/_$FASTCDR_PKG_NAME.BUILD_$PLAT_X64
        sed -i "s#@FASTRTPS_LIB@#$FASTRTPS_LIB_ARM64#g" $FASTRTPS_PKG_PATH/_$FASTCDR_PKG_NAME.BUILD_$PLAT_ARM64
        sed -i "s#@FASTRTPS_LIB@#$FASTRTPS_LIB_ARM32#g" $FASTRTPS_PKG_PATH/_$FASTCDR_PKG_NAME.BUILD_$PLAT_ARM32

        sed -i "s#@FASTRTPS_LIB@#$FASTRTPS_LIB_X64#g" $FASTRTPS_PKG_PATH/_$FASTRTPS_PKG_NAME.BUILD_$PLAT_X64
        sed -i "s#@FASTRTPS_LIB@#$FASTRTPS_LIB_ARM64#g" $FASTRTPS_PKG_PATH/_$FASTRTPS_PKG_NAME.BUILD_$PLAT_ARM64
        sed -i "s#@FASTRTPS_LIB@#$FASTRTPS_LIB_ARM32#g" $FASTRTPS_PKG_PATH/_$FASTRTPS_PKG_NAME.BUILD_$PLAT_ARM32

        #workspace scripte
        sed -i "s#@FASTRTPS_PKG@#$PKG_R_PATH#g" $FASTRTPS_PKG_PATH/_${EX_WORKSPACK}_$PLAT_X64
        sed -i "s#@FASTRTPS_PKG@#$PKG_R_PATH#g" $FASTRTPS_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM64
        sed -i "s#@FASTRTPS_PKG@#$PKG_R_PATH#g" $FASTRTPS_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM32
        sed -i "s#@FASTRTPS_PKG_PATH@#$FASTRTPS_INSTALL_X64_PATH#g" $FASTRTPS_PKG_PATH/_${EX_WORKSPACK}_$PLAT_X64
        sed -i "s#@FASTRTPS_PKG_PATH@#$FASTRTPS_INSTALL_ARM64_PATH#g" $FASTRTPS_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM64
        sed -i "s#@FASTRTPS_PKG_PATH@#$FASTRTPS_INSTALL_ARM_PATH#g" $FASTRTPS_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM32

        good "fast rtps & cdr installed OK"
    else
        info "fast rtps & cdr already OK"
    fi
}

function install_uuid() {
    info "check & install uuid library"
    cd $TOPDIR

    if [ ! -d $UUID_PKG_PATH ]; then
        info "copy uuid library"
        cp -rf $TOPDIR/$EXTERNAL_DIR/$UUID_PKG_NAME $EXTERNAL_BUILD_PATH

        cp $TOPDIR/$EXTERNAL_DIR/$UUID_PKG_NAME/$UUID_PKG_NAME.BUILD $UUID_PKG_PATH/_$UUID_PKG_NAME.BUILD_$PLAT_X64
        cp $TOPDIR/$EXTERNAL_DIR/$UUID_PKG_NAME/$UUID_PKG_NAME.BUILD $UUID_PKG_PATH/_$UUID_PKG_NAME.BUILD_$PLAT_ARM64
        cp $TOPDIR/$EXTERNAL_DIR/$UUID_PKG_NAME/$UUID_PKG_NAME.BUILD $UUID_PKG_PATH/_$UUID_PKG_NAME.BUILD_$PLAT_ARM32

        cp $TOPDIR/$EXTERNAL_DIR/$UUID_PKG_NAME/$EX_WORKSPACK $UUID_PKG_PATH/_${EX_WORKSPACK}_$PLAT_X64
        cp $TOPDIR/$EXTERNAL_DIR/$UUID_PKG_NAME/$EX_WORKSPACK $UUID_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM64
        cp $TOPDIR/$EXTERNAL_DIR/$UUID_PKG_NAME/$EX_WORKSPACK $UUID_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM32

        # Build scripts
        sed -i "s#@UUID_LIB@#$UUID_LIB_X64#g" $UUID_PKG_PATH/_$UUID_PKG_NAME.BUILD_$PLAT_X64
        sed -i "s#@UUID_LIB@#$UUID_LIB_ARM64#g" $UUID_PKG_PATH/_$UUID_PKG_NAME.BUILD_$PLAT_ARM64
        sed -i "s#@UUID_LIB@#$UUID_LIB_ARM32#g" $UUID_PKG_PATH/_$UUID_PKG_NAME.BUILD_$PLAT_ARM32

        #cat $UUID_PKG_PATH/_$UUID_PKG_NAME.BUILD_$PLAT_ARM32

        # Workspace scripte
        sed -i "s#@UUID_PKG@#$PKG_R_PATH#g" $UUID_PKG_PATH/_${EX_WORKSPACK}_$PLAT_X64
        sed -i "s#@UUID_PKG@#$PKG_R_PATH#g" $UUID_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM64
        sed -i "s#@UUID_PKG@#$PKG_R_PATH#g" $UUID_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM32
        sed -i "s#@UUID_PKG_PATH@#$UUID_INSTALL_X64_PATH#g" $UUID_PKG_PATH/_${EX_WORKSPACK}_$PLAT_X64
        sed -i "s#@UUID_PKG_PATH@#$UUID_INSTALL_ARM64_PATH#g" $UUID_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM64
        sed -i "s#@UUID_PKG_PATH@#$UUID_INSTALL_ARM_PATH#g" $UUID_PKG_PATH/_${EX_WORKSPACK}_$PLAT_ARM32

        rm $UUID_PKG_PATH/$UUID_PKG_NAME.BUILD
        rm $UUID_PKG_PATH/$EX_WORKSPACK

        cp $TOPDIR/$EXTERNAL_DIR/$UUID_PKG_NAME/$EX_WORKSPACK $UUID_PKG_PATH/${EX_WORKSPACK}
    else
        info "uuid already installed"
    fi

    #download & decompress
    if [ ! -e $UUID_PKG_PATH/$UUID_PKG_VERBOSE ]; then
        info "download uuid library"
        wget -nv $REMOTE_HTTP_SERVER/$UUID_PKG_NAME/$UUID_PKG_VERBOSE -P $UUID_PKG_PATH
        if [ $? -ne 0 ]; then
            error "can not get $UUID_PKG_VERBOSE"
            return -1
        fi
        tar -xzf $UUID_PKG_PATH/$UUID_PKG_VERBOSE -C $UUID_PKG_PATH
    fi

    if [ ! -f $UUID_PKG_PATH/.compiled ]; then
        info "compile uuid for multi-platform"

        #make buildout dir
        mkdir -p $UUID_PKG_PATH/$UUID_PKG_VERSION/$PLAT_X64
        mkdir -p $UUID_PKG_PATH/$UUID_PKG_VERSION/$PLAT_ARM32
        mkdir -p $UUID_PKG_PATH/$UUID_PKG_VERSION/$PLAT_ARM64

        info "start to compile x64 uuid library!"
        cd $UUID_PKG_PATH/$UUID_PKG_VERSION/$PLAT_X64
        ../configure --prefix=$UUID_INSTALL_X64_PATH > /dev/null
        good "config uuid x86 library ok!"
        [[ $? -eq 0 ]] && make > /dev/null && make install > /dev/null

        [[ $? -eq 0 ]] && good "compile x64 uuid x86 library OK" ||
            error "compile x86 uuid library NG"

        info "start to compile aarch uuid library!"
        cd $UUID_PKG_PATH/$UUID_PKG_VERSION/$PLAT_ARM64
        echo $AARCH_TOOL_CHAIN_DIR/$AARCH64_TOOL_CHAIN-gcc
        ../configure --prefix=$UUID_INSTALL_ARM64_PATH --host=arm-linux\
            CC=$AARCH_TOOL_CHAIN_DIR/${AARCH64_TOOL_CHAIN}gcc\
            CXX=$AARCH_TOOL_CHAIN_DIR/${AARCH64_TOOL_CHAIN}g++ > /dev/null
        good "config uuid aarch library ok!"
        [[ $? -eq 0 ]] && make > /dev/null && make install > /dev/null && touch \
            $UUID_PKG_PATH/.compiled

        [[ $? -eq 0 ]] && good "compile aarch uuid library OK" ||
            error "compile aarch uuid library NG"

        info "UUID installed OK"
    else
        info "UUID already OK"
    fi
}

function install_ros() {
    cd $TOPDIR

    if [ ! -d $BUILD_PATH/ros ];then
        mkdir -p $BUILD_PATH/ros
    fi

    if [ ! -d $BUILD_PATH/ros ] ||
        [ ! -f $BUILD_PATH/ros/.ROS_READY ]; then
        info "install melodic ros-x64 lib"
        rm -fr $BUILD_PATH/ros
        mkdir -p $BUILD_PATH/ros
        cd $BUILD_PATH/ros
        wget -nv $REMOTE_HTTP_SERVER/melodic/melodic.tar.gz
        if [ $? -ne 0 ]; then
            error "can not get melodic.tar.gz!"
            return -1
        fi
        tar zxf melodic.tar.gz
        good "ros install OK, path: $ROS_PATH"
        touch $BUILD_PATH/ros/.ROS_READY
    else
        info "ros has been installed"
    fi
}

function choose_config() {
    info "Mastiff ws configuration"
    log "\n       Which one would you like? [default 2]: "
    echo -e $STYLE_LRED
    echo -e ${MASTIFF_CONFIG_ARRAYS[@]} | xargs -n 1 | sed "=" | sed "N;s/\n/. /"\
        | sed 's/^/        /g'

    local INDEX
    while true; do
        echo -e "$STYLE_BLUE$STYLE_BOLD"
        read -p "         Choose workspace ---> " INDEX
        echo -e "$STYLE_NORMAL"
        INDEX=$((${INDEX:-0} - 1))

        if [ "$INDEX" -eq -1 ]; then
            good "Lunching for default workspace..."
            MASTIFF_BUILD_CONF="${MASTIFF_CONFIG_ARRAYS[1]}"
            [ -n "$MASTIFF_BUILD_CONF" ] && break
        fi

        if echo $INDEX | grep -vq [^0-9]; then
            MASTIFF_BUILD_CONF="${MASTIFF_CONFIG_ARRAYS[$INDEX]}"
            [ -n "$MASTIFF_BUILD_CONF" ] && break
        fi

        error "Choice not available.  Please try again."
    done
}

function config_bazel_ws() {
    info "workspace setup, input config: $1"

    MASTIFF_CONFIG_ARRAYS=( $(cd ${TOPDIR}/; ls bzel/WORKSPACE.* | \
        grep "WORKSPACE" | sort))

    unset MASTIFF_BUILD_CONF

    MASTIFF_DEFCONFIG_ARRAY_LEN=${#MASTIFF_CONFIG_ARRAYS[@]}

    #for ((i = 0; i < $MASTIFF_DEFCONFIG_ARRAY_LEN; i++))
    #do
        #warning "${MASTIFF_CONFIG_ARRAYS[$i]}"
    #done

    case $MASTIFF_DEFCONFIG_ARRAY_LEN in
        0)
            error "No available workspace config!"
            ;;
        *)
            choose_config $1
            ;;
    esac

    info use $MASTIFF_BUILD_CONF
    cd $TOPDIR

    if [ -e $MASTIFF_BUILD_CONF ]; then
        cp $MASTIFF_BUILD_CONF bzel/WORKSPACE
        ln -sf bzel/WORKSPACE WORKSPACE

        sed -i "s#@urls_com_google_protobuf@#$EXTERNAL_BUILD_PATH/urls_com_google_protobuf/v3.14.0.tar.gz#g" bzel/WORKSPACE
        sed -i "s#@com_github_google_glog@#$EXTERNAL_BUILD_PATH/com_github_google_glog/v0.4.0.tar.gz#g" bzel/WORKSPACE
        sed -i "s#@com_google_googletest@#$EXTERNAL_BUILD_PATH/com_google_googletest/release-1.7.0.zip#g" bzel/WORKSPACE
        sed -i "s#@rules_python@#$EXTERNAL_BUILD_PATH/rules_python/rules_python-0.1.0.tar.gz#g" bzel/WORKSPACE
        sed -i "s#@com_github_grpc_grpc@#$EXTERNAL_BUILD_PATH/com_github_grpc_grpc/v1.30.0.tar.gz#g" bzel/WORKSPACE
        sed -i "s#@bazel_skylib@#$EXTERNAL_BUILD_PATH/bazel_skylib/bazel-skylib-1.0.3.tar.gz#g" bzel/WORKSPACE
        sed -i "s#@zlib@#$EXTERNAL_BUILD_PATH/zlib/v1.2.11.tar.gz#g" bzel/WORKSPACE
        sed -i "s#@com_github_gflags_gflags@#$EXTERNAL_BUILD_PATH/com_github_gflags_gflags/v2.2.2.tar.gz#g" bzel/WORKSPACE

        sed -i "s#@THIRDPARTY_PATH@#$EXTERNAL_BUILD_PATH#g" bzel/WORKSPACE
        sed -i "s#$BUILD_PATH#$BUILD_DIR#g" bzel/WORKSPACE

        sed -i "s#@ROS_WORKSPACE@#$ROS_PATH#g" bzel/WORKSPACE

        sed -i "s#@REMOTE_HTTP_SERVER@#$REMOTE_HTTP_SERVER#g" bzel/WORKSPACE
    else
        info "$MASTIFF_BUILD_CONF not exist!!"
    fi

    #if [ ! -f $TOPDIR/external/slam/repositories.bzl ]; then
    if [ 1 ]; then
        SOURCE=$(echo "$MASTIFF_BUILD_CONF" | awk '{print substr($0, 16, 12)}')

        if [[ "$SOURCE" = "module.mstf" ]]; then
            #warning "generate mastiff slam repos"
            cp $TOPDIR/external/slam/repositories.bzl.tpl $TOPDIR/external/slam/repositories.bzl
            sed -i "s#@REMOTE_HTTP_SERVER@#$REMOTE_HTTP_SERVER#g" $TOPDIR/external/slam/repositories.bzl

            #warning "generate mastiff nginx repos"
            cp $TOPDIR/external/fringe_nginx/bazel/fringe_nginx.bzl.tpl $TOPDIR/external/fringe_nginx/bazel/fringe_nginx.bzl
            sed -i "s#@REMOTE_HTTP_SERVER@#$REMOTE_HTTP_SERVER#g" $TOPDIR/external/fringe_nginx/bazel/fringe_nginx.bzl

            #warning "generate mastiff nlohmann json repos"
            cp $TOPDIR/external/nlohmann_json/workspace.bzl.tpl $TOPDIR/external/nlohmann_json/workspace.bzl
            sed -i "s#@REMOTE_HTTP_SERVER@#$REMOTE_HTTP_SERVER#g" $TOPDIR/external/nlohmann_json/workspace.bzl

            #warning "generate mastiff valgrind repos"
            cp $TOPDIR/external/valgrind/bazel/valgrind.bzl.tpl $TOPDIR/external/valgrind/bazel/valgrind.bzl
            sed -i "s#@REMOTE_HTTP_SERVER@#$REMOTE_HTTP_SERVER#g" $TOPDIR/external/valgrind/bazel/valgrind.bzl
        elif [[ "$SOURCE" = "module.ext" ]]; then
            #warning "generate external slam repos"
            cp $TOPDIR/external/slam/repositories1.bzl.tpl $TOPDIR/external/slam/repositories.bzl
            sed -i "s#@REMOTE_HTTP_SERVER@#$REMOTE_HTTP_SERVER#g" $TOPDIR/external/slam/repositories.bzl

            #warning "generate mastiff nginx repos"
            cp $TOPDIR/external/fringe_nginx/bazel/fringe_nginx.bzl.tpl $TOPDIR/external/fringe_nginx/bazel/fringe_nginx.bzl
            sed -i "s#@REMOTE_HTTP_SERVER@#$REMOTE_HTTP_SERVER#g" $TOPDIR/external/fringe_nginx/bazel/fringe_nginx.bzl

            #warning "generate mastiff nlohmann json repos"
            cp $TOPDIR/external/nlohmann_json/workspace.bzl.tpl $TOPDIR/external/nlohmann_json/workspace.bzl
            sed -i "s#@REMOTE_HTTP_SERVER@#$REMOTE_HTTP_SERVER#g" $TOPDIR/external/nlohmann_json/workspace.bzl

            #warning "generate mastiff valgrind repos"
            cp $TOPDIR/external/valgrind/bazel/valgrind.bzl.tpl $TOPDIR/external/valgrind/bazel/valgrind.bzl
            sed -i "s#@REMOTE_HTTP_SERVER@#$REMOTE_HTTP_SERVER#g" $TOPDIR/external/valgrind/bazel/valgrind.bzl
        fi
        good "generate thirdparty source repos OK!"
    else
        error "$MASTIFF_BUILD_CONF not exist!!"
        return -1
    fi
}

function show_env_success() {
    sync

    success "execute mstf.sh OK"
    log "$CHANGE_LINE       Enjoy Mastiff!$CHANGE_LINE"
}

#
######
#
unset LD_LIBRARY_PATH

check_remote_server

if [ $? -eq 0 ]; then
    install_toolchain
    if [ $? -eq 0 ]; then
        install_bazel
        if [ $? -eq 0 ]; then
            install_fastrtps
            if [ $? -eq 0 ]; then
                install_uuid
                if [ $? -eq 0 ]; then
                    install_ros
                    if [ $? -eq 0 ]; then
                        config_bazel_ws $1
                        [[ $? -eq 0 ]] && show_env_success ||
                            error "config bazel workspace"
                    else
                        error "install ROS"
                    fi
                else
                    error "uuid install"
                fi
            else
                error "install fastrtsp"
            fi
        else
            error "bazel install"
        fi
    else
        error "toolchains install"
    fi
else
    error "file sever check failed!"
fi

cd $TOPDIR
