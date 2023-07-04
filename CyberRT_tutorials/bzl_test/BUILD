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
# bazel run //CyberRT_tutorials/bzl_test:write_file_by_write_string.txt
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

# bazel run //CyberRT_tutorials/bzl_test:gen_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_hello_world_by_template",
    #generate gen_hello_world_by_template.c
    user = "Lihu",
    greeting = "Handsome",
    template = ":hello.cc.tpl",
)
# bazel run //CyberRT_tutorials/bzl_test:gen_hello_monkey_by_template
gen_file_by_micro_template(
    name = "gen_hello_monkey_by_template",
    #generate gen_hello_monkey_by_template.c
    user = "Tarzen",
    greeting = "Strong",
    template = ":hello.cc.tpl",
)
# bazel run //CyberRT_tutorials/bzl_test:gen_hello_god_by_template
gen_file_by_micro_template(
    name = "gen_hello_god_by_template",
    #generate gen_hello_god_by_template.c
    user = "God",
    greeting = "All men are created equal",
    template = ":hello.cc.tpl",
)
# bazel run //CyberRT_tutorials/bzl_test:hello_by_template_bin -c opt
# bazel run //CyberRT_tutorials/bzl_test:hello_by_template_bin -c dbg
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
# bazel run //CyberRT_tutorials/bzl_test:gen_hello_in_build
genrule(
    name = "gen_hello_in_build",
    outs = ["gen_hello_in_build.txt"],
    cmd = "echo Hello World! > $@",
)
### ----- 3, generate by genrule in BUILD ----- ###

### ----- 4, write file by bzl macro ----- ###
# bazel run //CyberRT_tutorials/bzl_test:write_file_by_bzl_macro_gen
gen_file_by_macro(
    name = "write_file_by_bzl_macro",
    content = "Hi Monkey, Get Funky!",
)
### ----- 4, write file by bzl macro ----- ###

### ----- 5, write file by bzl macro ----- ###
# bazel run //CyberRT_tutorials/bzl_test:get_git_version_in_build
genrule(
    name = "get_git_version_in_build",
    outs = ["_version"],
    local = 1,
    #cmd = "git rev-parse HEAD > $@",
    cmd = "env",
)
### ----- 5, write file by bzl macro ----- ###

### ----- 6, get git version by bzl macro ----- ###
# bazel run //CyberRT_tutorials/bzl_test:get_git_version_by_micro
git_version(
    name = "get_git_version_by_micro",
    outs = ["_iversion"],
)
### ----- 6, get git version by bzl macro ----- ###

### ----- 7, platform constraint ----- ###
# bazel run //CyberRT_tutorials/bzl_test:my_rocks --platforms=//CyberRT_tutorials/bzl_test:marble_platform
sh_binary(
    name = "my_rocks",
    srcs = select({
        ":basalt": ["pyroxene.sh"],
        ":marble": ["calcite.sh"],
        ":igneous": ["igneous.sh"],
        ":metamorphic": ["metamorphic.sh"],
        "//conditions:default": ["feldspar.sh"],
    }),
)

config_setting(
    name = "basalt",
    constraint_values = [
        ":black", ":igneous",
    ],
)

config_setting(
    name = "marble",
    constraint_values = [
        ":white", ":metamorphic",
    ],
)

# constraint_setting acts as an enum type, and constraint_value as an enum value.
constraint_setting(name = "color")
constraint_value(name = "black", constraint_setting = "color")
constraint_value(name = "white", constraint_setting = "color")
constraint_setting(name = "texture")
constraint_value(name = "smooth", constraint_setting = "texture")
constraint_setting(name = "type")
constraint_value(name = "igneous", constraint_setting = "type")
constraint_value(name = "metamorphic", constraint_setting = "type")

platform(
    name = "basalt_platform",
    constraint_values = [
        ":black", ":igneous",
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
sh_binary(
    name = "your_rocks",
    srcs = select({
        ":igneous": ["igneous.sh"],
        ":metamorphic": ["metamorphic.sh"],
    }),
)
### ----- 7, platform constraint ----- ###

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
