# This package contains rule definitions for mastiff project.

load("//external/toolchain:cc_mastiff_def_toolchain.bzl",
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
# //external/toolchain:toolchain_type.
toolchain_type(name = "mstf_toolchain_type")

# 1, Toolchain that targets os Linux cpu x64, executes on os Linux cpu x64
# //external/toolchain:target_linux_x64_exe_linux_cpu_x64_toolchain_cc
toolchain(
    name = "target_linux_x64_exe_linux_cpu_x64_toolchain_cc",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
        ":mstf_lang_cc",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
        ":mstf_lang_cc",
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
# //external/toolchain:target_host_exe_host_toolchain
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

# 3, Toolchain that targets os Linux cpu x64, executes on os Linux cpu aarch64
# //external/toolchain:target_linux_x64_exe_linux_cpu_aarch64_toolchain_cc
toolchain(
    name = "target_linux_x64_exe_linux_cpu_aarch64_toolchain_cc",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:aarch_64",
        ":mstf_lang_cc",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
        ":mstf_lang_cc",
    ],
    toolchain = ":linux_x64_linux_arm64_mstf_toolchain",
    toolchain_type = ":mstf_toolchain_type",
)

mstf_toolchain(
    name = "linux_x64_linux_arm64_mstf_toolchain",
    executing_on_cpu = "aarch64",
    executing_on_os = "linux",
    targetting_cpu = "x86_64",
    targetting_os = "linux",
)

# 4, Toolchain that targets Windows cpu x64, and executes on Linux cpu x64
# //external/toolchain:target_windows_x64_exe_linux_x64_toolchain_cc
toolchain(
    name = "target_windows_x64_exe_linux_x64_toolchain_cc",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//os:x86_64",
    ],
    target_compatible_with = [
        "@platforms//os:windows",
        "@platforms//os:x86_64",
    ],
    toolchain = ":windows_x64_linux_x64mstf_toolchain",
    toolchain_type = ":mstf_toolchain_type",
)

mstf_toolchain(
    name = "windows_x64_linux_x64mstf_toolchain",
    executing_on_cpu = "x86_64",
    executing_on_os = "linux",
    targetting_cpu = "x86_64",
    targetting_os = "windows",
)

# 5, Toolchain that targets os Linux cpu x64, executes on os Linux cpu x64
# //external/toolchain:target_linux_x64_exe_linux_cpu_x64_toolchain_c
toolchain(
    name = "target_linux_x64_exe_linux_cpu_x64_toolchain_c",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
        ":mstf_lang_c",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
        ":mstf_lang_c",
    ],
    toolchain = ":linux_x64_linux_x64_mstf_toolchain",
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

####
# tool chains definition
load(":mastiff_build_conf.bzl",
    "cc_mastiff_toolchain_config_aarch64_641",
    "cc_mastiff_toolchain_config_aarch64_93",
    "cc_mastiff_toolchain_config_aarch64_94",
    "cc_mastiff_toolchain_config_aarch64_103",
    "cc_mastiff_toolchain_config_aarch64_113",
    "cc_mastiff_toolchain_config_arm32",
    "cc_mastiff_toolchain_config_x86_64"
)

filegroup(name = "empty")

# aarch gcc 6.4.1
cc_toolchain_suite(
    name = "mastiff_cross_gcc_suite_641",
    toolchains = {
        "aarch64": ":mastiff_aarch64_toolchain",
    },
)
cc_mastiff_toolchain_config_aarch64_641(
    name = "mastiff_aarch64_toolchain_config",
    disable_static_cc_toolchains = True
)
cc_toolchain(
    name = "mastiff_aarch64_toolchain",
    toolchain_identifier = "mastiff-aarch64-toolchain",
    toolchain_config = ":mastiff_aarch64_toolchain_config",
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

# aarch gcc 9.3
cc_toolchain_suite(
    name = "mastiff_cross_gcc_suite_93",
    toolchains = {
        "aarch64": ":mastiff_aarch64_93_toolchain",
    },
)
cc_mastiff_toolchain_config_aarch64_93(name = "mastiff_aarch64_93_toolchain_config")
cc_toolchain(
    name = "mastiff_aarch64_93_toolchain",
    toolchain_identifier = "mastiff-aarch_93-toolchain",
    toolchain_config = ":mastiff_aarch64_93_toolchain_config",
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

# aarch gcc 9.4
cc_toolchain_suite(
    name = "mastiff_cross_gcc_suite_94",
    toolchains = {
        "aarch64": ":mastiff_aarch64_94_toolchain",
    },
)
cc_mastiff_toolchain_config_aarch64_94(name = "mastiff_aarch64_94_toolchain_config")
cc_toolchain(
    name = "mastiff_aarch64_94_toolchain",
    toolchain_identifier = "mastiff-aarch_94-toolchain",
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

# aarch gcc 10.3
cc_toolchain_suite(
    name = "mastiff_cross_gcc_suite_103",
    toolchains = {
        "aarch64": ":mastiff_aarch64_103_toolchain",
    },
)
cc_mastiff_toolchain_config_aarch64_103(name = "mastiff_aarch64_103_toolchain_config")
cc_toolchain(
    name = "mastiff_aarch64_103_toolchain",
    toolchain_identifier = "mastiff-aarch_103-toolchain",
    toolchain_config = ":mastiff_aarch64_103_toolchain_config",
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

# aarch gcc 11.3
cc_toolchain_suite(
    name = "mastiff_cross_gcc_suite_113",
    toolchains = {
        "aarch64": ":mastiff_aarch64_113_toolchain",
    },
)
cc_mastiff_toolchain_config_aarch64_113(name = "mastiff_aarch64_113_toolchain_config")
cc_toolchain(
    name = "mastiff_aarch64_113_toolchain",
    toolchain_identifier = "mastiff-aarch_113-toolchain",
    toolchain_config = ":mastiff_aarch64_113_toolchain_config",
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

# arm gcc gnueabihf
cc_toolchain_suite(
    name = "mastiff_cross_gcc_suite_arm",
    toolchains = {
        "aarch64": ":mastiff_arm_toolchain",
    },
)
cc_mastiff_toolchain_config_arm32(name = "mastiff_arm_toolchain_config")
cc_toolchain(
    name = "mastiff_arm_toolchain",
    toolchain_identifier = "mastiff-arm32-toolchain",
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

# x64 gcc on host
cc_toolchain_suite(
    name = "mastiff_gcc_suite_local",
    toolchains = {
        "x86_64": ":mastiff_x86_64_toolchain",
    },
)
cc_mastiff_toolchain_config_x86_64(
    name = "mastiff_x86_64_toolchain_config",
    disable_static_cc_toolchains = True
)
cc_toolchain(
    name = "mastiff_x86_64_toolchain",
    toolchain_identifier = "mastiff-x86_64-toolchain",
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
