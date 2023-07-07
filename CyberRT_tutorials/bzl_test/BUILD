# Test bazel tools features for tutorial
#

load("@rules_cc//cc:defs.bzl",
     "cc_library",
     "cc_binary",
     "cc_test")

load(":test.bzl",
     "write_file_by_string",
     "gen_file_by_micro_template",
     "gen_file_by_macro",
     "git_version")

package(default_testonly = True)

print("BUILD for bazel test")

### ----- 0, bzl_test package ----- ###
# bazel run //CyberRT_tutorials/bzl_test:bzl_test
cc_library(
    name = "bzl_test",
    deps = [
        ":write_file_by_write_string.txt",
        ":hello_by_template_bin",
        ":gen_hello_in_build",
        ":write_file_by_bzl_macro",
        ":get_git_version_in_build",
        ":get_git_version_by_micro",
    ],
)
### ----- 0, bzl_test package ----- ###

### -----  1, generate file by writing string ----- ###
# bazel build //CyberRT_tutorials/bzl_test:write_file_by_write_string.txt
write_file_by_string(
    name = "write_file_by_write_string.txt",
    usernam = "Lihu",
    say = "Are U OK?"
)
### -----  1, generate file by writing string ----- ###

### ----- 2, generate file by template ----- ###
config_setting(
    name = "gen_hello_world",
    values = {
        #"cpu": "x86_64", #--cpu x86_64
        "compilation_mode": "opt", #-c opt
        #"force_pic": "false", #--noforce_pic
    },
)
config_setting(
    name = "gen_hello_monkey",
    values = {
        #"cpu": "aarch64_94", #--cpu aarch64_94
        "compilation_mode": "dbg", #-c dbg
        #"force_pic": "true", #--force_pic=1
        "define": "foo=bar", #--define foo=bar
    },
)

# bazel build //CyberRT_tutorials/bzl_test:gen_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_hello_world_by_template",
    #generate gen_hello_world_by_template.c
    user = "Lihu",
    greeting = "Handsome",
    template = ":hello.cc.tpl",
)
# bazel build //CyberRT_tutorials/bzl_test:gen_hello_monkey_by_template
gen_file_by_micro_template(
    name = "gen_hello_monkey_by_template",
    #generate gen_hello_monkey_by_template.c
    user = "Tarzen",
    greeting = "Strong",
    template = ":hello.cc.tpl",
)
# bazel build //CyberRT_tutorials/bzl_test:gen_hello_god_by_template
gen_file_by_micro_template(
    name = "gen_hello_god_by_template",
    #generate gen_hello_god_by_template.c
    user = "God",
    greeting = "All men are created equal",
    template = ":hello.cc.tpl",
)
# bazel run //CyberRT_tutorials/bzl_test:hello_by_template_bin -c opt
# bazel run //CyberRT_tutorials/bzl_test:hello_by_template_bin -c dbg
# bazel run //CyberRT_tutorials/bzl_test:hello_by_template_bin --define foo=bar  -c dbg
# bazel run //CyberRT_tutorials/bzl_test:hello_by_template_bin
cc_binary(
    name = "hello_by_template_bin",
    srcs = select({
        "gen_hello_world" : [":gen_hello_world_by_template"], #gen_hello_world_by_template.cc
        "gen_hello_monkey" : [":gen_hello_monkey_by_template"], #gen_hello_monkey_by_template.cc
        "//conditions:default": [":gen_hello_god_by_template"], #gen_hello_god_by_template.cc
    }),
)
### ----- 2, generate file by template ----- ###

### ----- 3, generate by genrule in BUILD ----- ###
# bazel build //CyberRT_tutorials/bzl_test:gen_hello_in_build
genrule(
    name = "gen_hello_in_build",
    outs = ["gen_hello_in_build.txt"],
    cmd = "echo Hello World! > $@",
)
### ----- 3, generate by genrule in BUILD ----- ###

### ----- 4, write file by bzl macro ----- ###
# bazel build //CyberRT_tutorials/bzl_test:write_file_by_bzl_macro_gen
gen_file_by_macro(
    name = "write_file_by_bzl_macro",
    content = "Hi Monkey, Get Funky!",
)
### ----- 4, write file by bzl macro ----- ###

### ----- 5, write file by bzl macro ----- ###
# bazel build //CyberRT_tutorials/bzl_test:get_git_version_in_build
genrule(
    name = "get_git_version_in_build",
    outs = ["_version"],
    local = 1,
    #cmd = "git rev-parse HEAD > $@",
    cmd = "env",
)
### ----- 5, write file by bzl macro ----- ###

### ----- 6, get git version by bzl macro ----- ###
# bazel build //CyberRT_tutorials/bzl_test:get_git_version_by_micro
git_version(
    name = "get_git_version_by_micro",
    outs = ["_iversion"],
)
### ----- 6, get git version by bzl macro ----- ###

### ----- 7, platform constraint ----- ###
# bazel run //CyberRT_tutorials/bzl_test:my_rocks
# bazel run //CyberRT_tutorials/bzl_test:my_rocks --platforms=//CyberRT_tutorials/bzl_test:basalt_platform
# bazel run //CyberRT_tutorials/bzl_test:my_rocks --platforms=//CyberRT_tutorials/bzl_test:marble_platform
sh_binary(
    name = "my_rocks",
    srcs = select({
        ":basalt": ["pyroxene.sh"],
        ":marble": ["calcite.sh"],
        "//conditions:default": ["feldspar.sh"],
    }),
)

config_setting(
    name = "basalt",
    constraint_values = [
        ":black",
        ":igneous",
    ],
)

config_setting(
    name = "marble",
    constraint_values = [
        ":white",
        ":metamorphic",
    ],
)

# constraint_setting acts as an enum type, and constraint_value as an enum value.
constraint_setting(name = "color")
constraint_value(name = "black", constraint_setting = "color")
constraint_value(name = "white", constraint_setting = "color")
constraint_setting(name = "texture")
constraint_value(name = "smooth", constraint_setting = "texture")
constraint_value(name = "coarse", constraint_setting = "texture")
constraint_setting(name = "type")
constraint_value(name = "igneous", constraint_setting = "type")
constraint_value(name = "metamorphic", constraint_setting = "type")

platform(
    name = "basalt_platform",
    constraint_values = [
        ":black",
        ":igneous",
    ],
)
platform(
    name = "marble_platform",
    constraint_values = [
        ":white",
        ":smooth",
        ":metamorphic",
    ],
)

# choose directly by reading constraint_value s
# bazel run //CyberRT_tutorials/bzl_test:your_rocks --platforms=//CyberRT_tutorials/bzl_test:marble_platform
# bazel run //CyberRT_tutorials/bzl_test:your_rocks --platforms=//CyberRT_tutorials/bzl_test:basalt_platform
sh_binary(
    name = "your_rocks",
    srcs = select({
        ":igneous": ["igneous.sh"],
        ":metamorphic": ["metamorphic.sh"],
    }),
)
### ----- 7, platform constraint ----- ###

### ----- 8, Configurable attributes ----- ###
# bazel run //CyberRT_tutorials/bzl_test:sh_arm_lib
sh_binary(
    name = "sh_arm_lib",
    srcs = [
        "arm_lib.sh",
    ],
)
# bazel run //CyberRT_tutorials/bzl_test:sh_x86_dev_lib
sh_binary(
    name = "sh_x86_dev_lib",
    srcs = [
        "x86_dev_lib.sh",
    ],
)
# bazel run //CyberRT_tutorials/bzl_test:sh_generic_lib
sh_binary(
    name = "sh_generic_lib",
    srcs = [
        "generic_lib.sh",
    ],
)

# bazel run //CyberRT_tutorials/bzl_test:arm_lib
cc_library(
    name = "arm_lib",
    srcs = [
        "gen_hello_world_by_template",
    ],
)
# bazel run //CyberRT_tutorials/bzl_test:x86_dev_lib
cc_library(
    name = "x86_dev_lib",
    srcs = [
        "gen_hello_monkey_by_template",
    ],
)
# bazel run //CyberRT_tutorials/bzl_test:generic_lib
cc_library(
    name = "generic_lib",
    srcs = [
        "gen_hello_god_by_template",
    ],
)

constraint_setting(name = "exec_arch")
constraint_value(name = "aarm", constraint_setting = "exec_arch")
constraint_value(name = "xx64", constraint_setting = "exec_arch")
constraint_value(name = "cs_aarm", constraint_setting = "exec_arch")
constraint_value(name = "cs_xx64", constraint_setting = "exec_arch")
constraint_setting(name = "host_os")
constraint_value(name = "linux", constraint_setting = "host_os")
constraint_value(name = "ios", constraint_setting = "host_os")

# defines matching
config_setting(
    name = "arm_build",
    values = {
        "define": "exec_arch=aarm",
    },
)
config_setting(
    name = "x86_debug_build",
    values = {
        "define": "exec_arch=xx64",
    },
)

# constraints
config_setting(
    name = "arm_build_cs",
    constraint_values = [
        ":cs_aarm",
        ":linux",
    ],
)

config_setting(
    name = "x86_debug_build_cs",
    constraint_values = [
        ":cs_xx64",
        ":ios",
    ],
)

platform(
    name = "arm_build_cs_plat",
    constraint_values = [
        ":cs_aarm",
        ":linux",
    ],
)

platform(
    name = "x86_debug_build_cs_plat",
    constraint_values = [
        ":cs_xx64",
        ":ios",
    ],
)

# bazel run //CyberRT_tutorials/bzl_test:mybinary
# bazel run //CyberRT_tutorials/bzl_test:mybinary --define exec_arch=aarm
# bazel run //CyberRT_tutorials/bzl_test:mybinary --define exec_arch=xx64

# ??? TODO
# bazel run //CyberRT_tutorials/bzl_test:mybinary --platforms=//CyberRT_tutorials/bzl_test:arm_build_cs_plat
# bazel run //CyberRT_tutorials/bzl_test:mybinary --platforms=//CyberRT_tutorials/bzl_test:x86_debug_build_cs_plat

# ??? TODO
# bazel run //CyberRT_tutorials/bzl_test:mybinary --crosstool_top=@bazel_tools//tools/cpp:toolchain --host_crosstool_top=@bazel_tools//tools/cpp:toolchain --platforms=//CyberRT_tutorials/bzl_test:arm_build_cs_plat
# bazel run //CyberRT_tutorials/bzl_test:mybinary --crosstool_top=@bazel_tools//tools/cpp:toolchain --host_crosstool_top=@bazel_tools//tools/cpp:toolchain --platforms=//CyberRT_tutorials/bzl_test:x86_debug_build_cs_plat

cc_binary(
    name = "mybinary",
    deps = select({
        ":arm_build": [":arm_lib"],
        ":x86_debug_build": [":x86_dev_lib"],
        ":arm_build_cs": [":arm_lib"],
        ":x86_debug_build_cs": [":x86_dev_lib"],
        "//conditions:default": [":generic_lib"],
    }),
)
### ----- 8, Configurable attributes ----- ###

### ----- 9, Platforms configuration ----- ###
config_setting(
    name = "linux_arm",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:arm",
    ],
)

config_setting(
    name = "linux_ppc",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:ppc",
    ],
)

config_setting(
    name = "linux_aarch64",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:aarch64",
    ],
)

config_setting(
    name = "linux_x86_64",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
)

config_setting(
    name = "osx_arm64",
    constraint_values = [
        "@platforms//os:osx",
        "@platforms//cpu:aarch64",
    ],
)

config_setting(
    name = "osx_x86_64",
    constraint_values = [
        "@platforms//os:osx",
        "@platforms//cpu:x86_64",
    ],
)

config_setting(
    name = "windows_x86_64",
    constraint_values = [
        "@platforms//os:windows",
        "@platforms//cpu:x86_64",
    ],
)

config_setting(
    name = "android_arm",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:arm",
    ],
)

config_setting(
    name = "android_aarch64",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:aarch64",
    ],
)

platform(
    name = "pl_linux_arm",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:arm",
    ],
)

platform(
    name = "pl_linux_ppc",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:ppc",
    ],
)

platform(
    name = "pl_linux_aarch64",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:aarch64",
    ],
)

platform(
    name = "pl_linux_x86_64",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
)

platform(
    name = "pl_osx_arm64",
    constraint_values = [
        "@platforms//os:osx",
        "@platforms//cpu:aarch64",
    ],
)

platform(
    name = "pl_osx_x86_64",
    constraint_values = [
        "@platforms//os:osx",
        "@platforms//cpu:x86_64",
    ],
)

platform(
    name = "pl_windows_x86_64",
    constraint_values = [
        "@platforms//os:windows",
        "@platforms//cpu:x86_64",
    ],
)

platform(
    name = "pl_android_arm",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:arm",
    ],
)

platform(
    name = "pl_android_aarch64",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:aarch64",
    ],
)

# bazel build //CyberRT_tutorials/bzl_test:gen_linux_arm_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_linux_arm_hello_world_by_template",
    #generate gen_linux_arm_hello_world_by_template.c
    user = "ARM",
    greeting = "Platform ARM32, OS Linux",
    template = ":hello.cc.tpl",
)

# bazel build //CyberRT_tutorials/bzl_test:gen_linux_ppc_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_linux_ppc_hello_world_by_template",
    #generate gen_linux_ppc_hello_world_by_template.c
    user = "PPC",
    greeting = "Platform PPC, OS Linux",
    template = ":hello.cc.tpl",
)

# bazel build //CyberRT_tutorials/bzl_test:gen_linux_aarch64_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_linux_aarch64_hello_world_by_template",
    #generate gen_linux_aarch64_hello_world_by_template.c
    user = "AARCH64",
    greeting = "Platform AARCH64, OS Linux",
    template = ":hello.cc.tpl",
)

# bazel build //CyberRT_tutorials/bzl_test:gen_linux_x86_64_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_linux_x86_64_hello_world_by_template",
    #generate gen_linux_x86_64_hello_world_by_template.c
    user = "X64",
    greeting = "Platform X86_64, OS Linux",
    template = ":hello.cc.tpl",
)

# bazel build //CyberRT_tutorials/bzl_test:gen_osx_aarch64_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_osx_aarch64_hello_world_by_template",
    #generate gen_osx_aarch64_hello_world_by_template.c
    user = "AARCH64",
    greeting = "Platform AARCH64, OS OSX",
    template = ":hello.cc.tpl",
)

# bazel build //CyberRT_tutorials/bzl_test:gen_osx_x86_64_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_osx_x86_64_hello_world_by_template",
    #generate gen_osx_x86_64_hello_world_by_template.c
    user = "X64",
    greeting = "Platform X86_64, OS OSX",
    template = ":hello.cc.tpl",
)

# bazel build //CyberRT_tutorials/bzl_test:gen_windows_x86_64_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_windows_x86_64_hello_world_by_template",
    #generate gen_windows_x86_64_hello_world_by_template.c
    user = "X64",
    greeting = "Platform X86_64, OS Windows",
    template = ":hello.cc.tpl",
)

# bazel build //CyberRT_tutorials/bzl_test:gen_android_arm_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_android_arm_hello_world_by_template",
    #generate gen_android_arm_hello_world_by_template.c
    user = "ARM",
    greeting = "Platform ARM32, OS Android",
    template = ":hello.cc.tpl",
)

# bazel build //CyberRT_tutorials/bzl_test:gen_android_aarch64_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_android_aarch64_hello_world_by_template",
    #generate gen_android_aarch64_hello_world_by_template.c
    user = "AARCH64",
    greeting = "Platform Aarch64, OS Android",
    template = ":hello.cc.tpl",
)

# bazel build //CyberRT_tutorials/bzl_test:gen_xx_xxx_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_xx_xxx_hello_world_by_template",
    #generate gen_xx_xxx_hello_world_by_template.c
    user = "ARCH Unkown",
    greeting = "Platform ??, OS ??",
    template = ":hello.cc.tpl",
)

# bazel run //CyberRT_tutorials/bzl_test:platform_choose_bin
# bazel run //CyberRT_tutorials/bzl_test:platform_choose_bin --platforms=//CyberRT_tutorials/bzl_test:pl_linux_arm
# bazel run //CyberRT_tutorials/bzl_test:platform_choose_bin --platforms=//CyberRT_tutorials/bzl_test:pl_linux_ppc
# bazel run //CyberRT_tutorials/bzl_test:platform_choose_bin --platforms=//CyberRT_tutorials/bzl_test:pl_linux_aarch64
# bazel run //CyberRT_tutorials/bzl_test:platform_choose_bin --platforms=//CyberRT_tutorials/bzl_test:pl_linux_x86_64
# bazel run //CyberRT_tutorials/bzl_test:platform_choose_bin --platforms=//CyberRT_tutorials/bzl_test:pl_osx_arm64
# bazel run //CyberRT_tutorials/bzl_test:platform_choose_bin --platforms=//CyberRT_tutorials/bzl_test:pl_osx_x86_64
# bazel run //CyberRT_tutorials/bzl_test:platform_choose_bin --platforms=//CyberRT_tutorials/bzl_test:pl_windows_x86_64
# bazel run //CyberRT_tutorials/bzl_test:platform_choose_bin --platforms=//CyberRT_tutorials/bzl_test:pl_android_arm
# bazel run //CyberRT_tutorials/bzl_test:platform_choose_bin --platforms=//CyberRT_tutorials/bzl_test:pl_android_aarch64
cc_binary(
    name = "platform_choose_bin",
    srcs = select({
        "linux_arm": [":gen_linux_arm_hello_world_by_template"],
        "linux_ppc": [":gen_linux_ppc_hello_world_by_template"],
        "linux_aarch64": [":gen_linux_aarch64_hello_world_by_template"],
        "linux_x86_64": [":gen_linux_x86_64_hello_world_by_template"],
        "osx_arm64": [":gen_osx_aarch64_hello_world_by_template"],
        "osx_x86_64": [":gen_osx_x86_64_hello_world_by_template"],
        "windows_x86_64": [":gen_windows_x86_64_hello_world_by_template"],
        "android_arm": [":gen_android_arm_hello_world_by_template"],
        "android_aarch64": [":gen_android_aarch64_hello_world_by_template"],
        "//conditions:default": [":gen_xx_xxx_hello_world_by_template"],
    }),
)
### ----- 9, Platforms configuration ----- ###

### ----- 8, combining select()s ----- ###
# bazel run //CyberRT_tutorials/bzl_test:my_target
config_setting(
    name = "x86_mode",
    values = {
        "cpu": "x86_64", #--cpu x86_64
    },
)
config_setting(
    name = "armeabi_mode",
    values = {
        "cpu": "arm", #--cpu arm
    },
)
config_setting(
    name = "opt_mode",
    values = {
        "compilation_mode": "opt", #-c dbg
    },
)
config_setting(
    name = "dbg_mode",
    values = {
        "compilation_mode": "dbg", #-c dbg
    },
)
sh_binary(
    name = "my_target",
    srcs = ["always_include.sh"] +
    select({
        ":x86_mode": ["x86_src.sh"],
        ":armeabi_mode": ["armeabi_src.sh"],
    },
    no_match_error = "Please build with x86 or arm platform",
    ) +
    select({
        ":opt_mode": ["opt_extras.sh"],
        ":dbg_mode": ["dbg_extras.sh"],
    },
    no_match_error = "Please build with an opt or debug param",
    ),
)
### ----- 8, combining select()s ----- ###
