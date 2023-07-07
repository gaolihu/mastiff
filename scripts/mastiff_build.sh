#!/bin/bash

TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
source "${TOPDIR}/scripts/mastiff.bashrc"

BAZEL_BUILD_OPTS="--host_force_python=PY2 --python_version=PY2 --verbose_failures --jobs 8 --spawn_strategy=standalone"
#BAZEL_BUILD_REMOTE="--remote_cache=http://172.16.1.133/cache"
BAZEL_BUILD_REMOTE=""

PLAT=""

unset TOPDIR
TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"

function choose_pkg() {
    log "\n       Witch packages to build?"

    echo -e $STYLE_LRED
    echo ${BUILD_PKG_ARRAY[@]} | xargs -n 1 | sed "=" | sed "N;s/\n/. /"\
        | sed 's/^/        /g'

    echo -e "$STYLE_BLUE$STYLE_BOLD\n"

    local INDEX
    while true; do
        read -p "       Choose package ---> " INDEX
        INDEX=$((${INDEX:-0} - 1))

        if [ "$INDEX" -eq -1 ]; then
            error " Lunching for invalid configs..."
            unset BUILD_PKG

            break;
        fi
        if echo $INDEX | grep -vq [^0-9]; then
            BUILD_PKG="${BUILD_PKG_ARRAY[$INDEX]}"
            if [ "x$option" != x ]
            then
                BUILD_PKG=$BUILD_PKG:$option
            fi
            [ -n "$BUILD_PKG" ] && break
        fi

        error " Choice not available.  Please try again."

    done
    echo -e "$STYLE_NORMAL"
}

function build_pkg() {
    info "building package: [ $BUILD_PKG ]"
    local BUILD_CMD="bazel build $PLAT $BUILD_PKG $BAZEL_BUILD_OPTS $BAZEL_BUILD_REMOTE"
    info "$BUILD_CMD"
    $BUILD_CMD
}

option=$1

function copy_arm() {
    cp $PKG_R_PATH/fastrtps/_fastrtps.BUILD_arm32 $PKG_R_PATH/fastrtps/fastrtps.BUILD
    cp $PKG_R_PATH/fastrtps/_fastcdr.BUILD_arm32 $PKG_R_PATH/fastrtps/fastcdr.BUILD
    cp $PKG_R_PATH/fastrtps/_workspace.bzl_arm32 $PKG_R_PATH/fastrtps/workspace.bzl
    cp $PKG_R_PATH/uuid/_uuid.BUILD_arm32 $PKG_R_PATH/uuid/uuid.BUILD
    cp $PKG_R_PATH/uuid/_workspace.bzl_arm32 $PKG_R_PATH/uuid/workspace.bzl
}

function copy_aarch() {
    cp $PKG_R_PATH/fastrtps/_fastrtps.BUILD_aarch64 $PKG_R_PATH/fastrtps/fastrtps.BUILD
    cp $PKG_R_PATH/fastrtps/_fastcdr.BUILD_aarch64 $PKG_R_PATH/fastrtps/fastcdr.BUILD
    cp $PKG_R_PATH/fastrtps/_workspace.bzl_aarch64 $PKG_R_PATH/fastrtps/workspace.bzl
    cp $PKG_R_PATH/uuid/_uuid.BUILD_aarch64 $PKG_R_PATH/uuid/uuid.BUILD
    cp $PKG_R_PATH/uuid/_workspace.bzl_aarch64 $PKG_R_PATH/uuid/workspace.bzl
}

function copy_x86() {
    cp $PKG_R_PATH/fastrtps/_fastrtps.BUILD_x86_64 $PKG_R_PATH/fastrtps/fastrtps.BUILD
    cp $PKG_R_PATH/fastrtps/_fastcdr.BUILD_x86_64 $PKG_R_PATH/fastrtps/fastcdr.BUILD
    cp $PKG_R_PATH/fastrtps/_workspace.bzl_x86_64 $PKG_R_PATH/fastrtps/workspace.bzl
    cp $PKG_R_PATH/uuid/_uuid.BUILD_x86_64 $PKG_R_PATH/uuid/uuid.BUILD
    cp $PKG_R_PATH/uuid/_workspace.bzl_x86_64 $PKG_R_PATH/uuid/workspace.bzl
}

function main() {
    info "Mastiff project building!"

    log "\n       Which platform to compile for?"
    echo -e $STYLE_LRED
    echo -e "[options]:\n\tarm32 (arm)\n\tgcc6.4.1 (a*)\n\tx86_64 (x*)\n\tgcc9.3 (3*)\n\tgcc9.4 (4*)\n\tgcc10.3 (0*)\n\tgcc11.3 (1*)]\n"

    echo -e "$STYLE_BLUE$STYLE_BOLD"

    while true; do
        read -t 60 -p "       Choose compiler ---> " plat
        echo -e "\n$STYLE_NORMAL"
        case $plat in
            arm ) info "cross compile for arm32";
                PLAT="--config cross_arm32";
                copy_arm
                break;;
            [93]* ) info "cross compile for aarch64 9.3 compiler";
                PLAT="--config cross_arm64_93";
                copy_aarch
                break;;
            [4]* ) info "cross compile for aarch64 9.4 compiler";
                PLAT="--config cross_arm64_94";
                copy_aarch
                break;;
            [0]* ) info "cross compile for aarch64 10.3 compiler";
                PLAT="--config cross_arm64_103";
                copy_aarch
                break;;
            [1]* ) info "cross compile for aarch64 11.3 compiler";
                PLAT="--config cross_arm64_113";
                copy_aarch
                break;;
            [aA]* ) info "cross compile for aarch64";
                copy_aarch
                PLAT="--config cross_arm64_64";
                break;;
            [xX]* ) info "Use default platform: x86_64\n";
                PLAT="--config plat_x86";
                copy_x86
                break;;
            * ) info "Use default x64 platform: x86 64\n";
                PLAT="--config plat_x86";
                copy_x86
                break;;
            [eENno]* )
                exit;;
        esac
    done

    BUILD_PKG_ARRAY=(
        $(cd $TOPDIR; \
        find . -name BUILD |\
        sed  "/.build/d" |
        sed  "/.bzel/d" |
        sed  "/.cache/d" |
        sed  "/.cache_build/d" |
        sed  "/.build_cache/d" |
        sed  "/proto/d" |
        sed  "/cyber/d" |
        sed  "/tools/d" |
        sed  "/docker/d" |
        sed  "/cartographer_ros_msgs/d" |
        sed  "/external/d" |
        sed "s#\.#/#g" |\
        sed "s|/BUILD$||g" |\
        grep "$1" | sort)
    )

    BUILD_PKG_ARRAY_LEN=${#BUILD_PKG_ARRAY[@]}

    case $BUILD_PKG_ARRAY_LEN in
        0)
            error "No available package to be built!"
            exit -1
            ;;
        *)
            if [ "$1" = ${BUILD_PKG_ARRAY[0]} ]; then
                BUILD_PKG=$1
            else
                choose_pkg
            fi
            ;;
    esac

    [ -n "BUILD_PKG" ] || return

    build_pkg
}

main
