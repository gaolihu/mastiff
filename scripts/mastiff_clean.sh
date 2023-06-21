#!/bin/bash

TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
source "${TOPDIR}/scripts/mastiff.bashrc"

OPTIONS="${@:-allsave}"

function clean_log() {
    info "bazel info clean!"
    rm -fr $TOPDIR/INFO*
}

function clean_bazel() {
    info "bazel clean!"

    clean_log

    bazel clean
    bazel clean --expunge

    rm -fr $TOPDIR/bazel-*
    rm -fr $TOPDIR/mstf_build.sh
    rm -fr $TOPDIR/mstf_clean.sh
    rm -fr $TOPDIR/WORKSPACE
    rm -fr out/
}

function clean_bazdir() {
    info "bazel directory clean!"
    clean_bazel
    rm -fr ~/.cache_build/bazel
}

function clean_build() {
    info "build cache directory clean!"
    clean_bazdir

    rm -fr external/toolchain/cc_build_config.bzl
    rm -fr external/ros_ws/workspace.bzl
}

function clean_all() {
    info "totally all clean!"
    clean_bazdir
    clean_build
}

function clean_dft() {
    log "bazel clean usage:\n"
    log " option 1: log"
    log " option 2: bazel"
    log " option 3: bazdir"
    log " option 4: build"
    log " option 5: all\n"

    clean_bazdir
}

for option in ${OPTIONS}; do
    #echo "processing option: $option"
    case $option in
        log | 1 ) clean_log ;;
        bazel | 2 ) clean_bazel ;;
        bazdir | 3 ) clean_bazdir ;;
        build | 4 ) clean_build ;;
        all | 5 ) clean_all ;;
        * ) clean_dft ;;
    esac
done
