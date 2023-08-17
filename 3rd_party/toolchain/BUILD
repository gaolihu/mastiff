# This package contains rule definitions for mastiff project.

load("//3rd_party/toolchain:cc_mastiff_def_toolchain.bzl",
     "mstf_library", "mstf_toolchain")

# @local_config_platform will automatically detect host platform and
# generate relevant constraints.
load("@local_config_platform//:constraints.bzl", "HOST_CONSTRAINTS")
#HOST_CONSTRAINTS = [
    #'@platforms//cpu:x86_64',
    #'@platforms//os:linux',
#]

package(default_visibility = ["//visibility:public"])

# Toolchain type is used to distinguish toolchains per languages.
#
# By convention, toolchain_type targets are named "toolchain_type" and
# distinguished by their package path. So the full path for this would be
# //3rd_party/toolchain:toolchain_type.
toolchain_type(name = "mstf_toolchain_type")

#mstf x86
platform(
    name = "mstf_x64",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
)
#mstf aarch64
platform(
    name = "mstf_aarch64",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:aarch64",
    ],
)
#mstf arm
platform(
    name = "mstf_arm",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:arm",
    ],
)

# 1, Toolchain that targets os:Linux cpu:x64, executes on os:Linux cpu:x64
# //3rd_party/toolchain:target_linux_x64_exe_linux_cpu_x64_toolchain_cc
toolchain(
    name = "target_linux_x64_exe_linux_cpu_x64_toolchain_cc",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
        "@bazel_tools//tools/cpp:gcc",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
        "@bazel_tools//tools/cpp:gcc",
    ],
    toolchain = ":linux_x64_linux_x64_mstf_toolchain",
    toolchain_type = ":mstf_toolchain_type",
)
mstf_toolchain(
    name = "linux_x64_linux_x64_mstf_toolchain",
    executing_on_cpu = "x86_64",
    executing_on_os = "linux",
    targetting_cpu = "x86_64",
    targetting_os = "linux",
)

# 2, Naive toolchain that targets and executes on the current
# host platforms as autodetected by Bazel.
# //3rd_party/toolchain:target_host_exe_host_toolchain
toolchain(
    name = "target_host_exe_host_toolchain",
    exec_compatible_with = HOST_CONSTRAINTS,
    target_compatible_with = HOST_CONSTRAINTS,
    toolchain = ":host_host_mstf_toolchain",
    toolchain_type = ":mstf_toolchain_type",
)
mstf_toolchain(
    name = "host_host_mstf_toolchain",
    executing_on_cpu = "host",
    executing_on_os = "host",
    targetting_cpu = "host",
    targetting_os = "host",
)

# 3, Toolchain that targets os:Linux cpu:aarch64, executes on os:Linux cpu:x64
# //3rd_party/toolchain:target_linux_aarch64_exe_linux_cpu_x64_toolchain_cc
toolchain(
    name = "target_linux_aarch64_exe_linux_cpu_x64_toolchain_cc",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:aarch64",
    ],
    toolchain = ":aarch64_gcc94_toolchain",
    toolchain_type = ":mstf_toolchain_type",
)

# 4, Toolchain that targets os:Linux cpu:aarch64, executes on host
# //3rd_party/toolchain:target_linux_aarch64_exe_host_toolchain_cc
toolchain(
    name = "target_linux_aarch64_exe_host_toolchain_cc",
    exec_compatible_with = HOST_CONSTRAINTS,
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:aarch64",
    ],
    toolchain = ":aarch64_gcc94_toolchain",
    toolchain_type = ":mstf_toolchain_type",
)

# mastiff local constraints
constraint_setting(name = "mstf_lang_version")

constraint_value(
    name = "mstf_lang_c",
    constraint_setting = ":mstf_lang_version",
)

constraint_value(
    name = "mstf_lang_cc",
    constraint_setting = ":mstf_lang_version",
)

constraint_value(
    name = "mstf_lang_py",
    constraint_setting = ":mstf_lang_version",
)

constraint_value(
    name = "mstf_lang_sh",
    constraint_setting = ":mstf_lang_version",
)

############## legacy ###############
# tool chains definition
load(":mastiff_build_conf.bzl",
    "cc_mastiff_toolchain_config_aarch64_641",
    "cc_mastiff_toolchain_config_aarch64_94",
    "cc_mastiff_toolchain_config_arm32",
    "cc_mastiff_toolchain_config_x86_64"
)

filegroup(name = "empty")

# 1, arm gcc gnueabihf
cc_toolchain_suite(
    name = "mstf_cc_arm_gcc64_suite",
    toolchains = {
        "arm": ":arm_gcc64_toolchain",
    },
)
cc_mastiff_toolchain_config_arm32(name = "mastiff_arm_toolchain_config")
cc_toolchain(
    name = "arm_gcc64_toolchain",
    toolchain_identifier = "arm-gcc64-toolchain",
    toolchain_config = ":mastiff_arm_toolchain_config",
    all_files = ":empty",
    ar_files = ":empty",
    as_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

# 2, aarch gcc 6.4.1
cc_toolchain_suite(
    name = "mstf_cc_aarch64_gcc64_suite",
    toolchains = {
        "aarch64": ":aarch64_gcc64_toolchain",
    },
)
cc_mastiff_toolchain_config_aarch64_641(
    name = "mastiff_aarch64_gcc64_toolchain_config",
    disable_static_cc_toolchains = True
)
cc_toolchain(
    name = "aarch64_gcc64_toolchain",
    toolchain_identifier = "aarch64-gcc64-toolchain",
    toolchain_config = ":mastiff_aarch64_gcc64_toolchain_config",
    all_files = ":empty",
    ar_files = ":empty",
    as_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

# 3, aarch gcc 9.4
cc_toolchain_suite(
    name = "mstf_cc_aarch64_gcc94_suite",
    toolchains = {
        "aarch64": ":aarch64_gcc94_toolchain",
        #"aarch64|compiler": ":aarch64_gcc94_toolchain",

        "k8": ":mastiff_x86_64_toolchain",
        "#k8|gcc": ":mastiff_x86_64_toolchain",
    },
)
cc_mastiff_toolchain_config_aarch64_94(name = "mastiff_aarch64_94_toolchain_config")
cc_toolchain(
    name = "aarch64_gcc94_toolchain",
    toolchain_identifier = "aarch64_gcc94-toolchain",
    toolchain_config = ":mastiff_aarch64_94_toolchain_config",
    all_files = ":empty",
    ar_files = ":empty",
    as_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

# 4, x64 gcc on host
cc_toolchain_suite(
    name = "mastiff_gcc_suite_local",
    toolchains = {
        "k8": ":mastiff_x86_64_toolchain",
        "x86_64": ":mastiff_x86_64_toolchain",
    },
)
cc_mastiff_toolchain_config_x86_64(
    name = "mastiff_x86_64_toolchain_config",
    disable_static_cc_toolchains = True
)
cc_toolchain(
    name = "mastiff_x86_64_toolchain",
    toolchain_identifier = "x86_64-gcc-toolchain",
    toolchain_config = ":mastiff_x86_64_toolchain_config",
    all_files = ":empty",
    ar_files = ":empty",
    as_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)
