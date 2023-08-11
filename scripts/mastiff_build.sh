#!/bin/bash

TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
source "${TOPDIR}/scripts/mastiff.bashrc"

BAZEL_BUILD_OPTS="--host_force_python=PY2 --python_version=PY2 --verbose_failures --jobs 8 --spawn_strategy=standalone"
#BAZEL_BUILD_REMOTE="--remote_cache=http://xx.xx.xx.xx/cache"
BAZEL_BUILD_REMOTE=""

PLAT=""

PKG_RUN_CMD="##BZL.RUN:"
PKG_BUILD_CMD="##BZL.BUILD:"
PKG_BUILD_OPTS="##BZL.OPTS:"
PKG_DELETE_DFT_CMD="##BZL.RUN:"
PKG_DELETE_DFT_CMD="##BZL.RUN:"

BUILD_PROMPT="bazel info"

unset TOPDIR
TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"

function parse_sub_build() {
    sub_builds=$(grep -e "##BZL.BUILD:" ${SUB_PKG_PATH_ARRAY[$i]} | \
        sed 's|^##BZL.BUILD:||')
    sub_builds_seq=$(grep -n "##BZL.BUILD:" ${SUB_PKG_PATH_ARRAY[$i]} | \
        sed 's|:##BZL.BUILD:||')
    sub_build_array=($(echo $sub_builds | grep "$1" | sort))
    sub_build_seq_array=($(echo $sub_builds_seq | grep "$1" | sort))

    for ((m = 0; m < ${#sub_build_array[@]}; m++, j++))
    do
        sub_build_seq_array[$m]=$(echo ${sub_build_seq_array[$m]} | \
            sed "s|${sub_build_array[$m]}||")

        sub_build_packages_array[$m]="$base_pkg:${sub_build_array[$m]}"
        sub_build_packages_array[$m]="bazel~build~${sub_build_packages_array[m]}"
        #info "${sub_build_packages_array[$m]}"
    done

    #if [ $m -eq 0 ]; then
        #warning "no packges for building in ${SUB_PKG_PATH_ARRAY[$i]}"
    #fi
}

function parse_sub_run() {
    sub_runs=$(grep -e "##BZL.RUN:" ${SUB_PKG_PATH_ARRAY[$i]} | sed \
        's|^##BZL.RUN:||' | sed "s| |~|g" | sed "s|##BZL.OPTS:{|~|" | \
        sed "s|}||")
    sub_runs_seq=$(grep -n "##BZL.RUN:" ${SUB_PKG_PATH_ARRAY[$i]} | sed \
        's|:##BZL.RUN:||' | sed "s| |~|g" | sed "s|##BZL.OPTS:{|~|" | \
        sed "s|}||")
    sub_run_pkg_array=($(echo "$sub_runs" | grep "$1"))
    sub_run_pkg_seq_array=($(echo "$sub_runs_seq" | grep "$1"))

    for ((n = 0; n < ${#sub_run_pkg_array[@]}; n++, j++))
    do
        sub_run_pkg_seq_array[$n]=$(echo ${sub_run_pkg_seq_array[$n]} | \
            sed "s|${sub_run_pkg_array[$n]}||")

        sub_run_packages_array[n]="$base_pkg:${sub_run_pkg_array[$n]}"
        sub_run_packages_array[n]="bazel~run~${sub_run_packages_array[n]}"
        #info "${sub_run_packages_array[$n]}"
    done

    #if [ $n -eq 0 ]; then
        #warning "no packges for running in ${SUB_PKG_PATH_ARRAY[$i]}"
    #fi
}

function choose_pkg() {
    log "       Which package to build?"

    local m
    local n
    local r
    local t
    local o

    local ranked_cmd_and_package=()
    local sub_run_packages_array=()
    local sub_build_packages_array=()

    for ((i = 0, j = 0; i < $BUILD_PKG_ARRAY_LEN; i++))
    do
        base_pkg=$(echo ${BUILD_PKG_ARRAY[i]} | xargs -n 1 | sed "N;s/\n/. /")

        sub_del_all=$(grep -e "##BZL.DEL:all" ${SUB_PKG_PATH_ARRAY[$i]})
        sub_del_this=$(grep -e "##BZL.DEL:this" ${SUB_PKG_PATH_ARRAY[$i]})

        #warning "base: ${BUILD_PKG_ARRAY[$i]}"

        if [[ $sub_del_all ]]; then
            #delet all of the modules
            info "ignore all modules in: ${SUB_PKG_PATH_ARRAY[$i]}"
        else
            #partial delete
            if [[ $sub_del_this ]]; then
                #delete default module
                info "delete dft module in: ${SUB_PKG_PATH_ARRAY[$i]}"
            else
                #enable default module
                ranked_cmd_and_package[$j]="bazel~~build~$base_pkg"
                #good "[DFT] $j: ${ranked_cmd_and_package[$j]}"
                j=$(expr $j + 1)
            fi

            parse_sub_run
            parse_sub_build

            o=$(expr $m + $n)
            r=$(expr $j - $o)

            #good "${SUB_PKG_PATH_ARRAY[$i]}, from: $r, build: $m, run: $n, all: $o, ALL: $j"

            ty=0 #build queue idx
            tx=0 #run queue idx

            for ((; r < j; r++))
            do
                if [ ! ${sub_run_pkg_seq_array[$tx]} ]; then
                    ranked_cmd_and_package[r]=${sub_build_packages_array[ty]}
                    #good "RUN2, tx($tx): r($r), ${ranked_cmd_and_package[r]}"
                    ty=$(expr $ty + 1)
                    continue
                fi

                if [ ! ${sub_build_seq_array[$ty]} ]; then
                    ranked_cmd_and_package[r]=${sub_run_packages_array[tx]}
                    #good "BUILD2, ty($ty): r($r), ${ranked_cmd_and_package[r]}"
                    tx=$(expr $tx + 1)
                    continue
                fi

                if [ ${sub_run_pkg_seq_array[$tx]} -lt ${sub_build_seq_array[$ty]} ]; then
                    ranked_cmd_and_package[r]=${sub_run_packages_array[tx]}
                    #good "RUN1, tx($tx): r($r), ${ranked_cmd_and_package[r]}"
                    tx=$(expr $tx + 1)
                else
                    ranked_cmd_and_package[r]=${sub_build_packages_array[ty]}
                    #good "BUILD1, ty($ty): r($r), ${ranked_cmd_and_package[r]}"
                    ty=$(expr $ty + 1)
                fi
            done

            #ranking
            #for ((r = 0; r < o; r++))
            #do
                #line=$(echo ${cmd_and_package[$r]} | cut -d '~' -f 1)
                #echo "${ranked_cmd_and_package[$r]}"
            #done
        fi
    done
    echo -e $STYLE_MAGENTA

    for ((s = 0; s < ${#ranked_cmd_and_package[@]}; s++))
    do
        echo "${ranked_cmd_and_package[$s]}" | grep -q "bazel~~build"
        if [ $? -eq 0 ]; then
            echo ${ranked_cmd_and_package[s]} | xargs -n 1 | sed "N;s/\n/. /"\
                | sed 's/^/     /g' | \
                sed 's/~/ /g' | \
                sed -E -e "s#^#     [DEFAULT]    [ $(expr $s + 1) ]#"
        fi
        echo "${ranked_cmd_and_package[$s]}" | grep -q "bazel~build"
        #if [[ ${ranked_cmd_and_package[$s]} == "*bazel build*" ]]; then
        if [ $? -eq 0 ]; then
            echo ${ranked_cmd_and_package[s]} | xargs -n 1 | sed "N;s/\n/. /"\
                | sed 's/^/     /g' | \
                sed 's/~/ /g' | \
                sed -E -e "s#^#     [BUILD  ]    [ $(expr $s + 1) ]#"
        fi
        echo "${ranked_cmd_and_package[$s]}" | grep -q "bazel~run"
        if [ $? -eq 0 ]; then
            echo ${ranked_cmd_and_package[s]} | xargs -n 1 | sed "N;s/\n/. /"\
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
            BUILD_PKG=$(echo "${ranked_cmd_and_package[$INDEX]}" | sed 's/~/ /g')
            BUILD_PROMPT="${cmd_and_package[$INDEX]}"
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
    local BUILD_CMD="$BUILD_PKG $BUILD_PROMPT $PLAT $BAZEL_BUILD_OPTS $BAZEL_BUILD_REMOTE"
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
    echo -e "[options]:\n\t(arm) arm32\n\t(a*) gcc6.4.1\n\t(x*) x86_64\n\t(4*) gcc9.4"

    echo -e "$STYLE_BLUE$STYLE_BOLD"

    while true; do
        read -t 60 -p "       Choose compiler ---> " plat
        echo -e "$STYLE_NORMAL"
        case $plat in
            arm ) info "cross compile for arm32";
                PLAT="--config gcc_cross_arm32";
                copy_arm
                break;;
            [aA]* ) info "cross compile for aarch64";
                copy_aarch
                PLAT="--config gcc64_cross_aarch64";
                break;;
            [4]* ) info "cross compile for aarch64 9.4 compiler";
                PLAT="--config gcc94_cross_aarch64";
                copy_aarch
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
        sed  "/3rd_party/d" |
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
        sed  "/3rd_party/d" |
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
