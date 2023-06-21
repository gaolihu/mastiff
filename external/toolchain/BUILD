package(default_visibility = ["//visibility:public"])

load(
    ":cc_build_config.bzl",
    "cc_mastiff_toolchain_config_arm32",
    "cc_mastiff_toolchain_config_aarch64",
    "cc_mastiff_toolchain_config_aarch64_93",
    "cc_mastiff_toolchain_config_aarch64_94",
    "cc_mastiff_toolchain_config_aarch64_103",
    "cc_mastiff_toolchain_config_aarch64_113",
    "cc_mastiff_toolchain_config_x86_64"
)

filegroup(name = "empty")

cc_toolchain_suite(
    name = "mastiff_clang_suite_93",
    toolchains = {
        "aarch64": ":mastiff_aarch64_toolchain",
        "aarch64_93": ":mastiff_aarch64_93_toolchain",
        "arm32": ":mastiff_arm32_toolchain",
        "x86_64": ":mastiff_x86_64_toolchain",
    },
)

cc_toolchain_suite(
    name = "mastiff_clang_suite_94",
    toolchains = {
        "aarch64": ":mastiff_aarch64_toolchain",
        "aarch64_94": ":mastiff_aarch64_94_toolchain",
        "arm32": ":mastiff_arm32_toolchain",
        "x86_64": ":mastiff_x86_64_toolchain",
    },
)

cc_toolchain_suite(
    name = "mastiff_clang_suite_103",
    toolchains = {
        "aarch64": ":mastiff_aarch64_toolchain",
        "aarch64_103": ":mastiff_aarch64_103_toolchain",
        "arm32": ":mastiff_arm32_toolchain",
        "x86_64": ":mastiff_x86_64_toolchain",
    },
)

cc_toolchain_suite(
    name = "mastiff_clang_suite_113",
    toolchains = {
        "aarch64": ":mastiff_aarch64_toolchain",
        "aarch64_113": ":mastiff_aarch64_113_toolchain",
        "arm32": ":mastiff_arm32_toolchain",
        "x86_64": ":mastiff_x86_64_toolchain",
    },
)

cc_mastiff_toolchain_config_aarch64(
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

cc_mastiff_toolchain_config_arm32(name = "mastiff_arm32_toolchain_config")
cc_toolchain(
    name = "mastiff_arm32_toolchain",
    toolchain_identifier = "mastiff-arm32-toolchain",
    toolchain_config = ":mastiff_arm32_toolchain_config",
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
