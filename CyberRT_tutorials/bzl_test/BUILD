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

# 0, bzl_test package
# //CyberRT_tutorials/bzl_test:bzl_test
cc_library(
    name = "bzl_test",
    deps = [
        ":write_file_by_write_string.txt",
        ":hello_world_by_template",
        ":gen_hello_in_build",
        ":write_file_by_bzl_macro",
        ":get_git_version_in_build",
        ":get_git_version_by_micro",
    ],
)

#  1, generate file by writing string
# //CyberRT_tutorials/bzl_test:write_file_by_write_string.txt
write_file_by_string(
    name = "write_file_by_write_string.txt",
    usernam = "Lihu",
    say = "Are U OK?"
)

# 2, generate file by template
# //CyberRT_tutorials/bzl_test:gen_hello_world_by_template
gen_file_by_micro_template(
    name = "gen_hello_world_by_template", #generate gen_hello_world_by_template.c
    user = "Lihu",
    version = "exp",
    template = ":hello.cc.tpl",
)
# //CyberRT_tutorials/bzl_test:hello_world_by_template
cc_binary(
    name = "hello_world_by_template",
    srcs = [":gen_hello_world_by_template"], #gen_hello_world_by_template.cc
)

# 3, generate by genrule in BUILD
# //CyberRT_tutorials/bzl_test:gen_hello_in_build
genrule(
    name = "gen_hello_in_build",
    outs = ["gen_hello_in_build.txt"],
    cmd = "echo Hello World! > $@",
)

# 4, write file by bzl macro
# //CyberRT_tutorials/bzl_test:write_file_by_bzl_macro_gen
gen_file_by_macro(
    name = "write_file_by_bzl_macro",
    content = "Hi Monkey, Get Funky!",
)

# 5, write file by bzl macro
# //CyberRT_tutorials/bzl_test:get_git_version_in_build
genrule(
    name = "get_git_version_in_build",
    outs = ["_version"],
    local = 1,
    #cmd = "git rev-parse HEAD > $@",
    cmd = "env",
)

# 6, get git version by bzl macro
# //CyberRT_tutorials/bzl_test:get_git_version_by_micro
git_version(
    name = "get_git_version_by_micro",
    outs = ["_iversion"],
)
