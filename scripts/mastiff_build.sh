#!/bin/bash

TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
source "${TOPDIR}/scripts/mastiff.bashrc"

BAZEL_BUILD_OPTS="--host_force_python=PY2 --python_version=PY2 --verbose_failures --jobs 8 --spawn_strategy=standalone"
#BAZEL_BUILD_REMOTE="--remote_cache=http://xx.xx.xx.xx/cache"
BAZEL_BUILD_REMOTE=""

PLAT=""

BUILD_PROMPT="bazel info"
build_prompt_arrary=()

unset TOPDIR
TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"

function choose_pkg() {
    log "       Which packages to build?"

    local sub_packages=()

    for ((i = 0, j = 0; i < $BUILD_PKG_ARRAY_LEN; i++))
    do
        base_pack=$(echo ${BUILD_PKG_ARRAY[i]} | xargs -n 1 | sed "N;s/\n/. /")

        sub_del=$(grep -e "##BZL.DEL:" ${SUB_PKG_PATH_ARRAY[$i]} | sed 's|^##BZL.DEL:||')

        if [[ $sub_del != "this" ]]; then
            sub_packages[$j]="$base_pack"
            build_prompt_arrary[$j]=" bazel build"
            #good "self j: $j: ${sub_packages[$j]}"
            j=$(expr $j + 1)
        fi

        sub_build=$(grep -e "##BZL.BUILD:" ${SUB_PKG_PATH_ARRAY[$i]} | \
            sed 's|^##BZL.BUILD:||')
        sub_build_pkg=($(echo $sub_build | grep "$1" | sort))

        for ((m = 0; m < ${#sub_build_pkg[@]}; m++, j++))
        do
            sub_packages[$j]="$base_pack:${sub_build_pkg[$m]}"
            build_prompt_arrary[$j]="bazel build"
            #good "build j: $j: ${sub_packages[$j]}"
        done

        sub_run=$(grep -e "##BZL.RUN:" ${SUB_PKG_PATH_ARRAY[$i]} | sed \
            's|^##BZL.RUN:||' | sed "s| |~|g" | sed "s|##BZL.OPTS:{|~|" | \
            sed "s|}||")
        sub_run_pkg=($(echo "$sub_run" | grep "$1" | sort))
        #sub_run_pkg=(`echo "$sub_run" | tr '\n' ' '`)

        for ((n = 0; n < ${#sub_run_pkg[@]}; n++, j++))
        do
            sub_packages[$j]="$base_pack:${sub_run_pkg[$n]}"
            build_prompt_arrary[$j]="bazel run"
            #good "run j: $j: ${sub_packages[$j]}"
        done
    done

    echo -e $STYLE_MAGENTA

    for ((s = 0; s < ${#sub_packages[@]}; s++))
    do
        if [[ ${build_prompt_arrary[$s]} == " bazel build" ]]; then
            echo ${sub_packages[s]} | xargs -n 1 | sed "N;s/\n/. /"\
                | sed 's/^/     /g' | \
                sed 's/~/ /g' | \
                sed -E -e "s#^#     [DEFAULT]    [ $(expr $s + 1) ]#"
        fi
        if [[ ${build_prompt_arrary[$s]} == "bazel build" ]]; then
            echo ${sub_packages[s]} | xargs -n 1 | sed "N;s/\n/. /"\
                | sed 's/^/     /g' | \
                sed 's/~/ /g' | \
                sed -E -e "s#^#     [BUILD  ]    [ $(expr $s + 1) ]#"
        fi
        if [[ ${build_prompt_arrary[$s]} == "bazel run" ]]; then
            echo ${sub_packages[s]} | xargs -n 1 | sed "N;s/\n/. /"\
                | sed 's/^/     /g' | \
                sed 's/~/ /g' | \
                sed -E -e "s#^#     [RUN    ]    [ $(expr $s + 1) ]#"
        fi
    done

    echo -e "$STYLE_BLUE$STYLE_BOLD"

    local INDEX
    while true; do
        read -p "       Choose package ---> " INDEX
        INDEX=$((${INDEX:-0} - 1))

        if [ "$INDEX" -eq -1 ]; then
            error " Lunching for invalid configs..."
            unset BUILD_PKG

            break;
        fi

        #if echo $INDEX | grep -e -vq [^0-9]; then
        if [[ $INDEX =~ ^[0-9]+$ ]]; then
            BUILD_PKG=$(echo "${sub_packages[$INDEX]}" | sed 's/~/ /g')
            BUILD_PROMPT="${build_prompt_arrary[$INDEX]}"
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
    log "       [ $BUILD_PKG ]\n"
    local BUILD_CMD="$BUILD_PROMPT $PLAT $BUILD_PKG $BAZEL_BUILD_OPTS $BAZEL_BUILD_REMOTE"
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
    echo -e "[options]:\n\tarm32 (arm)\n\tgcc6.4.1 (a*)\n\tx86_64 (x*)\n\tgcc9.3 (3*)\n\tgcc9.4 (4*)\n\tgcc10.3 (0*)\n\tgcc11.3 (1*)]"

    echo -e "$STYLE_BLUE$STYLE_BOLD"

    while true; do
        read -t 60 -p "       Choose compiler ---> " plat
        echo -e "$STYLE_NORMAL"
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

    SUB_PKG_PATH_ARRAY=(
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
        grep "$1" | sort)
    )
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
