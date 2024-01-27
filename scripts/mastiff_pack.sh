#!/bin/bash

TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
source "${TOPDIR}/scripts/mastiff.bashrc"

TOPDIR=`pwd`
GCC_LIB=$TOPDIR/.cache_build/tool_chains/gcc-6.4.1-x86_64-arm-aarch64-linux-gnu/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/lib64/

cd $TOPDIR

rm -fr out/bin/
rm -fr out/lib/
rm -fr out/conf/
rm -fr out/btxml/
rm -fr out/dag/
rm -fr out/cyber/
rm -fr mastiff.tar

if [ ! -d out/bin ]; then
    mkdir -p out/bin/
fi

if [ ! -d out/lib ]; then
    mkdir -p out/lib/
fi

if [ ! -d out/conf ]; then
    mkdir -p out/conf/
fi

if [ ! -d out/btxml ]; then
    mkdir -p out/btxml/
fi

if [ ! -d out/dag ]; then
    mkdir -p out/dag/
fi

if [ ! -d out/nginx ]; then
    mkdir -p out/nginx/
fi

if [ ! -d out/cyber ]; then
    mkdir -p out/cyber/conf
fi

good "Init output files directory OK!"

plat=""

function choose_plat()
{
    info "Mastiff project packing!\n"
    info "Which platform do you want to pack?"

    while true; do
        echo -e " options:\n RK3308 (1/3308/08/RK3308)\n RK3566 (2/3566/66/RK3566)\n RK3588 (3/3588/88/RK3588)"
        read -t 60 -p " " plat
        case $plat in
            [1 | 1 | 3308 | 08 | RK3308] ) echo -e "\e[31;35m pack for RK3308\n";
                FOR_RK3308=1
                break;;
            [2 | 2 | 3566 | 66 | RK3566] ) echo -e "\e[31;35m pack for RK3566\n";
                FOR_RK3566=1
                break;;
            [3 | 3 | 3588 | 88 | RK3588] ) echo -e "\e[31;35m pack for RK3588\n";
                FOR_RK3588=1
                break;;
        esac
    done
}
#choose_plat

# chassis
cp modules/chassis/conf/ChassisConfigRK3588.pb.txt out/conf/ChassisConfig.conf -fr
if [ $? -eq 0 ]; then
    good "copy ChassisConfigRK3588 config OK!"
else
    warning "copy ChassisConfigRK3588 config NG!"
fi
cp bazel-bin/modules/chassis/chss_pkg out/bin/ -fr
if [ $? -eq 0 ]; then
    good "copy chss_pkg OK!"
else
    warning "copy chss_pkg NG!"
fi
# chassis so libs
find bazel-bin/modules/chassis/libchss_component.so.runfiles/ -name *.so | xargs cp -ft out/lib
if [ $? -eq 0 ]; then
    good "copy chassis so lib files OK!"
else
    warning "copy chassis so lib files NG!"
fi

# cambrian
cp modules/cambrian/conf/CambrianConfig.pb.txt out/conf/CambrianConfig.conf -fr
if [ $? -eq 0 ]; then
    good "copy CambrianConfig config OK!"
else
    warning "copy CambrianConfig config NG!"
fi
cp bazel-bin/modules/cambrian/camb_pkg out/bin/ -fr
if [ $? -eq 0 ]; then
    good "copy camb_pkg OK!"
else
    warning "copy camb_pkg NG!"
fi

# fastrtps & fast cdr
cp .cache_build/3rd_party/fastrtps/aarch64/fast-rtps-1.5.0-1/lib/*.so* out/lib/ -fr
if [ $? -eq 0 ]; then
    good "copy fastrtps library OK!"
else
    waring "copy fastrtps library NG!"
fi
cp .cache_build/3rd_party/uuid/libuuid-1.0.3/aarch64/lib/*.so* out/lib/ -fr
if [ $? -eq 0 ]; then
    good "copy uuid library OK!"
else
    waring "copy uuid library NG!"
fi

# cyber config
cp cyber/conf/* out/cyber/conf -fr
if [ $? -eq 0 ]; then
    good "copy CyberRT config files OK!"
else
    warning "copy CyberRT config files NG!"
fi

# cyber lib
find bazel-bin/cyber/ -not -path '*_objs/*' -name *.so | xargs cp -ft out/lib
if [ $? -eq 0 ]; then
    good "copy CyberRT so lib files OK!"
else
    warning "copy CyberRT so lib files NG!"
fi

# copy system lib
find $GCC_LIB -name libgomp.so* | xargs cp -fdt out/lib
if [ $? -eq 0 ]; then
    good "copy aarch so lib files OK!"
else
    warning "copy aarch so lib files NG!"
fi

# copy angstrong camera lib
#find bazel-bin/_solib_aarch64/ -not -path '*_objs/*' -name *.so | xargs cp -ft out/lib
find bazel-mastiff/external/angstrong_camera/lib/ -name "*.so" -exec cp -frd {} out/lib \;
if [ $? -eq 0 ]; then
    good "copy angstrong so lib files OK!"
else
    warning "copy angstrong so lib files NG!"
fi

# copy opencv libs
find bazel-mastiff/external/opencv4_8/lib/ -name "*.so*" -exec cp -frd {} out/lib/ \;
[[ $? -eq 0 ]] &&
    {
        good "copy openCV so lib files OK1!";
    } ||
    {
        warning "copy openCV so lib files NG!";
    }

# copy ffmpeg libs
find bazel-mastiff/external/ffmpeg_4_4_aarch/lib/ -name "*.so*" -exec cp -frd {} out/lib/ \;
[[ $? -eq 0 ]] &&
    {
        good "copy ffmpeg so lib files OK!";
    } ||
    {
        warning "copy ffmpeg so lib files NG!";
    }

# running scripts
cp scripts/mastiff_run_arm.sh out
if [ $? -eq 0 ]; then
    good "copy running script file OK!"
else
    warning "copy running script file NG!"
fi

# pack target
cd $TOPDIR/out
info "Create mastiff project tar-ball file!"
tar cf mastiff.tar *
mv mastiff.tar $TOPDIR
cd $TOPDIR

success "pack veuns finished!"
