load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load("@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
     "action_config",
    "feature",
    "flag_group",
    "flag_set",
    "tool",
    "tool_path",
    "with_feature_set",
)
load(
    "@bazel_tools//tools/build_defs/cc:action_names.bzl",
    _ASSEMBLE_ACTION_NAME = "ASSEMBLE_ACTION_NAME",
    _CLIF_MATCH_ACTION_NAME = "CLIF_MATCH_ACTION_NAME",
    _CPP_COMPILE_ACTION_NAME = "CPP_COMPILE_ACTION_NAME",
    _CPP_HEADER_PARSING_ACTION_NAME = "CPP_HEADER_PARSING_ACTION_NAME",
    _CPP_LINK_DYNAMIC_LIBRARY_ACTION_NAME = "CPP_LINK_DYNAMIC_LIBRARY_ACTION_NAME",
    _CPP_LINK_EXECUTABLE_ACTION_NAME = "CPP_LINK_EXECUTABLE_ACTION_NAME",
    _CPP_LINK_NODEPS_DYNAMIC_LIBRARY_ACTION_NAME = "CPP_LINK_NODEPS_DYNAMIC_LIBRARY_ACTION_NAME",
    _CPP_MODULE_CODEGEN_ACTION_NAME = "CPP_MODULE_CODEGEN_ACTION_NAME",
    _CPP_MODULE_COMPILE_ACTION_NAME = "CPP_MODULE_COMPILE_ACTION_NAME",
    _C_COMPILE_ACTION_NAME = "C_COMPILE_ACTION_NAME",
    _LINKSTAMP_COMPILE_ACTION_NAME = "LINKSTAMP_COMPILE_ACTION_NAME",
    _LTO_BACKEND_ACTION_NAME = "LTO_BACKEND_ACTION_NAME",
    _PREPROCESS_ASSEMBLE_ACTION_NAME = "PREPROCESS_ASSEMBLE_ACTION_NAME",
)

all_compile_actions = [
    _C_COMPILE_ACTION_NAME,
    _CPP_COMPILE_ACTION_NAME,
    _LINKSTAMP_COMPILE_ACTION_NAME,
    _ASSEMBLE_ACTION_NAME,
    _PREPROCESS_ASSEMBLE_ACTION_NAME,
    _CPP_HEADER_PARSING_ACTION_NAME,
    _CPP_MODULE_COMPILE_ACTION_NAME,
    _CPP_MODULE_CODEGEN_ACTION_NAME,
    _CLIF_MATCH_ACTION_NAME,
    _LTO_BACKEND_ACTION_NAME,
]

all_cpp_compile_actions = [
    _CPP_COMPILE_ACTION_NAME,
    _LINKSTAMP_COMPILE_ACTION_NAME,
    _CPP_HEADER_PARSING_ACTION_NAME,
    _CPP_MODULE_COMPILE_ACTION_NAME,
    _CPP_MODULE_CODEGEN_ACTION_NAME,
    _CLIF_MATCH_ACTION_NAME,
]

preprocessor_compile_actions = [
    _C_COMPILE_ACTION_NAME,
    _CPP_COMPILE_ACTION_NAME,
    _LINKSTAMP_COMPILE_ACTION_NAME,
    _PREPROCESS_ASSEMBLE_ACTION_NAME,
    _CPP_HEADER_PARSING_ACTION_NAME,
    _CPP_MODULE_COMPILE_ACTION_NAME,
    _CLIF_MATCH_ACTION_NAME,
]

codegen_compile_actions = [
    _C_COMPILE_ACTION_NAME,
    _CPP_COMPILE_ACTION_NAME,
    _LINKSTAMP_COMPILE_ACTION_NAME,
    _ASSEMBLE_ACTION_NAME,
    _PREPROCESS_ASSEMBLE_ACTION_NAME,
    _CPP_MODULE_CODEGEN_ACTION_NAME,
    _LTO_BACKEND_ACTION_NAME,
]

all_link_actions = [
    _CPP_LINK_EXECUTABLE_ACTION_NAME,
    _CPP_LINK_DYNAMIC_LIBRARY_ACTION_NAME,
    _CPP_LINK_NODEPS_DYNAMIC_LIBRARY_ACTION_NAME,
]

def _impl_arm(ctx):
    tool_paths = [
        tool_path(
            name = "addr2line",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@addr2line",
        ),
        tool_path(
            name = "ar",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@ar",
        ),
        tool_path(
            name = "as",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@as",
        ),
        tool_path(
            name = "c++",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@c++",
        ),
        tool_path(
            name = "c++filt",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@c++filt",
        ),
        tool_path(
            name = "cpp",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@cpp",
        ),
        tool_path(
            name = "dwp",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@dwp",
        ),
        tool_path(
            name = "elfedit",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@elfedit",
        ),
        tool_path(
            name = "g++",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@g++",
        ),
        tool_path(
            name = "gcc",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@gcc",
        ),
        tool_path(
            name = "gccar",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@gcc-ar",
        ),
        tool_path(
            name = "gcc-nm",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@gcc-nm",
        ),
        tool_path(
            name = "gcc-ranlib",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@gcc-ranlib",
        ),
        tool_path(
            name = "gcov",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@gcov",
        ),
        tool_path(
            name = "gcov-dump",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@gcov-dump",
        ),
        tool_path(
            name = "gcov-tool",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@gcov-tool",
        ),
        tool_path(
            name = "gdb",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@gdb",
        ),
        tool_path(
            name = "gfortran",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@gfortran",
        ),
        tool_path(
            name = "gprof",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@gprof",
        ),
        tool_path(
            name = "ld",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@ld",
        ),
        tool_path(
            name = "ld.bfd",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@ld.bfd",
        ),
        tool_path(
            name = "ld.gold",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@ld.gold",
        ),
        tool_path(
            name = "nm",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@nm",
        ),
        tool_path(
            name = "objcopy",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@objcopy",
        ),
        tool_path(
            name = "objdump",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@objdump",
        ),
        tool_path(
            name = "ranlib",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@ranlib",
        ),
        tool_path(
            name = "readelf",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@readelf",
        ),
        tool_path(
            name = "size",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@size",
        ),
        tool_path(
            name = "strings",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@strings",
        ),
        tool_path(
            name = "strip",
            path = "@PATH_TO_GCC_ARM@/@GCC_ARCH_ARM@strip",
        ),
    ]

    default_link_flags_feature = feature(
        name = "default_link_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-lstdc++",
                            "-Wl,-z,relro,-z,now",
                            "-no-canonical-prefixes",
                            "-pass-exit-codes",
                        ],
                    ),
                ],
            ),
        flag_set(
            actions = all_link_actions,
            flag_groups = [flag_group(flags = ["-Wl,--gc-sections"])],
            with_features = [with_feature_set(features = ["opt"])],
            ),
    ],
    )

    supports_dynamic_linker_feature = feature(name = "supports_dynamic_linker", enabled = True)
    supports_pic_feature = feature(name = "supports_pic", enabled = True)
    objcopy_embed_flags_feature = feature(
        name = "objcopy_embed_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = ["objcopy_embed_data"],
                flag_groups = [flag_group(flags = ["-I", "binary"])],
            )
        ],
    )

    unfiltered_compile_flags_feature = feature(
        name = "unfiltered_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-no-canonical-prefixes",
                            "-fno-canonical-system-headers",
                            "-Wno-builtin-macro-redefined",
                            "-D__DATE__=\"redacted\"",
                            "-D__TIMESTAMP__=\"redacted\"",
                            "-D__TIME__=\"redacted\"",
                        ],
                    ),
                ],
            ),
        ],
    )
    default_compile_flags_feature = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-U_FORTIFY_SOURCE",
                            "-D_FORTIFY_SOURCE=1",
                            "-fstack-protector",
                            "-Wall",
                            "-O2",
                            "-Wunused-but-set-parameter",
                            "-Wno-free-nonheap-object",
                            "-fno-omit-frame-pointer",
                        ],
                    ),
                ],
            ),
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [flag_group(flags = ["-g"])],
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-g0",
                            "-O2",
                            "-DNDEBUG",
                            "-ffunction-sections",
                            "-fdata-sections",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
            flag_set(
                actions = [
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [flag_group(flags = ["-std=c++1z"])],
            ),
        ],
    )
    opt_feature = feature(name = "opt")
    dbg_feature = feature(name = "dbg")
    user_compile_flags_feature = feature(
        name = "user_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = ["%{user_compile_flags}"],
                        iterate_over = "user_compile_flags",
                        expand_if_available = "user_compile_flags",
                    ),
                ],
            ),
        ],
    )
    sysroot_feature = feature(
        name = "sysroot",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                    _CPP_LINK_EXECUTABLE_ACTION_NAME,
                    _CPP_LINK_DYNAMIC_LIBRARY_ACTION_NAME,
                    _CPP_LINK_NODEPS_DYNAMIC_LIBRARY_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                    flags = ["--sysroot=%{sysroot}"],
                    expand_if_available = "sysroot",
                    ),
                ],
            ),
        ],
    )
    objcopy_embed_data_action = action_config(
        action_name = "objcopy_embed_data",
        enabled = True,
        tools = [tool(path = "/usr/bin/objcopy")],
    )
    features = [
        default_compile_flags_feature,
        default_link_flags_feature,
        supports_dynamic_linker_feature,
        supports_pic_feature,
        objcopy_embed_flags_feature,
        opt_feature,
        dbg_feature,
        user_compile_flags_feature,
        sysroot_feature,
        unfiltered_compile_flags_feature,
    ]
    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        features = features,
        cxx_builtin_include_directories = [
            "@PATH_TO_GCC_ARM@/../lib/gcc/aarch64-linux-gnu/6.3.1/include",
            "@PATH_TO_GCC_ARM@/../lib/gcc/aarch64-linux-gnu/6.3.1/include-fixed",
            "@PATH_TO_GCC_ARM@/../lib/gcc/aarch64-linux-gnu/6.3.1/install-tools/include",
            "@PATH_TO_GCC_ARM@/../lib/gcc/aarch64-linux-gnu/6.3.1/plugin/include",
            "@PATH_TO_GCC_ARM@/../aarch64-linux-gnu/include",
            "@PATH_TO_GCC_ARM@/../aarch64-linux-gnu/libc/usr/include",
            "@PATH_TO_GCC_ARM@/../include",
        ],
        toolchain_identifier = "arm-gcc64-toolchain",
        host_system_name = "local",
        target_system_name = "local",
        target_cpu = "arm32",
        target_libc = "unknown",
        compiler = "gcc",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
    )

def _impl_aarch(ctx):
    tool_paths = [
        tool_path(
            name = "addr2line",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@addr2line",
        ),
        tool_path(
            name = "ar",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@ar",
        ),
        tool_path(
            name = "as",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@as",
        ),
        tool_path(
            name = "c++",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@c++",
        ),
        tool_path(
            name = "c++filt",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@c++filt",
        ),
        tool_path(
            name = "cpp",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@cpp",
        ),
        tool_path(
            name = "dwp",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@dwp",
        ),
        tool_path(
            name = "elfedit",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@elfedit",
        ),
        tool_path(
            name = "g++",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@g++",
        ),
        tool_path(
            name = "gcc",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@gcc",
        ),
        tool_path(
            name = "gccar",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@gcc-ar",
        ),
        tool_path(
            name = "gcc-nm",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@gcc-nm",
        ),
        tool_path(
            name = "gcc-ranlib",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@gcc-ranlib",
        ),
        tool_path(
            name = "gcov",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@gcov",
        ),
        tool_path(
            name = "gcov-dump",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@gcov-dump",
        ),
        tool_path(
            name = "gcov-tool",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@gcov-tool",
        ),
        tool_path(
            name = "gdb",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@gdb",
        ),
        tool_path(
            name = "gfortran",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@gfortran",
        ),
        tool_path(
            name = "gprof",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@gprof",
        ),
        tool_path(
            name = "ld",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@ld",
        ),
        tool_path(
            name = "ld.bfd",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@ld.bfd",
        ),
        tool_path(
            name = "ld.gold",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@ld.gold",
        ),
        tool_path(
            name = "nm",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@nm",
        ),
        tool_path(
            name = "objcopy",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@objcopy",
        ),
        tool_path(
            name = "objdump",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@objdump",
        ),
        tool_path(
            name = "ranlib",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@ranlib",
        ),
        tool_path(
            name = "readelf",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@readelf",
        ),
        tool_path(
            name = "size",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@size",
        ),
        tool_path(
            name = "strings",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@strings",
        ),
        tool_path(
            name = "strip",
            path = "@PATH_TO_GCC_AARCH@/@GCC_ARCH_AARCH@strip",
        ),
    ]

    default_link_flags_feature = feature(
        name = "default_link_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-lstdc++",
                            "-Wl,-z,relro,-z,now",
                            "-no-canonical-prefixes",
                            "-pass-exit-codes",
                        ],
                    ),
                ],
            ),
        flag_set(
            actions = all_link_actions,
            flag_groups = [flag_group(flags = ["-Wl,--gc-sections"])],
            with_features = [with_feature_set(features = ["opt"])],
            ),
    ],
    )

    supports_dynamic_linker_feature = feature(name = "supports_dynamic_linker", enabled = True)
    supports_pic_feature = feature(name = "supports_pic", enabled = True)
    objcopy_embed_flags_feature = feature(
        name = "objcopy_embed_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = ["objcopy_embed_data"],
                flag_groups = [flag_group(flags = ["-I", "binary"])],
            )
        ],
    )

    unfiltered_compile_flags_feature = feature(
        name = "unfiltered_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-no-canonical-prefixes",
                            "-fno-canonical-system-headers",
                            "-Wno-builtin-macro-redefined",
                            "-D__DATE__=\"redacted\"",
                            "-D__TIMESTAMP__=\"redacted\"",
                            "-D__TIME__=\"redacted\"",
                        ],
                    ),
                ],
            ),
        ],
    )
    default_compile_flags_feature = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-U_FORTIFY_SOURCE",
                            "-D_FORTIFY_SOURCE=1",
                            "-fstack-protector",
                            "-Wall",
                            "-O2",
                            "-Wunused-but-set-parameter",
                            "-Wno-free-nonheap-object",
                            "-fno-omit-frame-pointer",
                        ],
                    ),
                ],
            ),
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [flag_group(flags = ["-g"])],
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-g0",
                            "-O2",
                            "-DNDEBUG",
                            "-ffunction-sections",
                            "-fdata-sections",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
            flag_set(
                actions = [
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [flag_group(flags = ["-std=c++1z"])],
            ),
        ],
    )
    opt_feature = feature(name = "opt")
    dbg_feature = feature(name = "dbg")
    user_compile_flags_feature = feature(
        name = "user_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = ["%{user_compile_flags}"],
                        iterate_over = "user_compile_flags",
                        expand_if_available = "user_compile_flags",
                    ),
                ],
            ),
        ],
    )
    sysroot_feature = feature(
        name = "sysroot",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                    _CPP_LINK_EXECUTABLE_ACTION_NAME,
                    _CPP_LINK_DYNAMIC_LIBRARY_ACTION_NAME,
                    _CPP_LINK_NODEPS_DYNAMIC_LIBRARY_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                    flags = ["--sysroot=%{sysroot}"],
                    expand_if_available = "sysroot",
                    ),
                ],
            ),
        ],
    )
    objcopy_embed_data_action = action_config(
        action_name = "objcopy_embed_data",
        enabled = True,
        tools = [tool(path = "/usr/bin/objcopy")],
    )
    features = [
        default_compile_flags_feature,
        default_link_flags_feature,
        supports_dynamic_linker_feature,
        supports_pic_feature,
        objcopy_embed_flags_feature,
        opt_feature,
        dbg_feature,
        user_compile_flags_feature,
        sysroot_feature,
        unfiltered_compile_flags_feature,
    ]
    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        features = features,
        cxx_builtin_include_directories = [
            "@PATH_TO_GCC_AARCH@/../lib/gcc/aarch64-linux-gnu/6.3.1/include",
            "@PATH_TO_GCC_AARCH@/../lib/gcc/aarch64-linux-gnu/6.3.1/include-fixed",
            "@PATH_TO_GCC_AARCH@/../lib/gcc/aarch64-linux-gnu/6.3.1/install-tools/include",
            "@PATH_TO_GCC_AARCH@/../lib/gcc/aarch64-linux-gnu/6.3.1/plugin/include",
            "@PATH_TO_GCC_AARCH@/../aarch64-linux-gnu/include",
            "@PATH_TO_GCC_AARCH@/../aarch64-linux-gnu/libc/usr/include",
            "@PATH_TO_GCC_AARCH@/../include",
        ],
        toolchain_identifier = "aarch64-gcc64-toolchain",
        host_system_name = "local",
        target_system_name = "local",
        target_cpu = "aarch64",
        target_libc = "unknown",
        compiler = "gcc",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
    )

def _impl_aarch94(ctx):
    tool_paths = [
        tool_path(
            name = "addr2line",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@addr2line",
        ),
        tool_path(
            name = "ar",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@ar",
        ),
        tool_path(
            name = "as",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@as",
        ),
        tool_path(
            name = "c++",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@c++",
        ),
        tool_path(
            name = "c++filt",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@c++filt",
        ),
        tool_path(
            name = "cpp",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@cpp",
        ),
        tool_path(
            name = "dwp",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@dwp",
        ),
        tool_path(
            name = "elfedit",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@elfedit",
        ),
        tool_path(
            name = "g++",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@g++",
        ),
        tool_path(
            name = "gcc",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@gcc",
        ),
        tool_path(
            name = "gccar",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@gcc-ar",
        ),
        tool_path(
            name = "gcc-nm",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@gcc-nm",
        ),
        tool_path(
            name = "gcc-ranlib",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@gcc-ranlib",
        ),
        tool_path(
            name = "gcov",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@gcov",
        ),
        tool_path(
            name = "gcov-dump",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@gcov-dump",
        ),
        tool_path(
            name = "gcov-tool",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@gcov-tool",
        ),
        tool_path(
            name = "gdb",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@gdb",
        ),
        tool_path(
            name = "gfortran",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@gfortran",
        ),
        tool_path(
            name = "gprof",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@gprof",
        ),
        tool_path(
            name = "ld",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@ld",
        ),
        tool_path(
            name = "ld.bfd",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@ld.bfd",
        ),
        tool_path(
            name = "ld.gold",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@ld.gold",
        ),
        tool_path(
            name = "nm",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@nm",
        ),
        tool_path(
            name = "objcopy",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@objcopy",
        ),
        tool_path(
            name = "objdump",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@objdump",
        ),
        tool_path(
            name = "ranlib",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@ranlib",
        ),
        tool_path(
            name = "readelf",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@readelf",
        ),
        tool_path(
            name = "size",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@size",
        ),
        tool_path(
            name = "strings",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@strings",
        ),
        tool_path(
            name = "strip",
            path = "@PATH_TO_GCC_AARCH94@/@GCC_ARCH_AARCH94@strip",
        ),
    ]

    default_link_flags_feature = feature(
        name = "default_link_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-lstdc++",
                            "-Wl,-z,relro,-z,now",
                            "-no-canonical-prefixes",
                            "-pass-exit-codes",
                        ],
                    ),
                ],
            ),
        flag_set(
            actions = all_link_actions,
            flag_groups = [flag_group(flags = ["-Wl,--gc-sections"])],
            with_features = [with_feature_set(features = ["opt"])],
            ),
    ],
    )

    supports_dynamic_linker_feature = feature(name = "supports_dynamic_linker", enabled = True)
    supports_pic_feature = feature(name = "supports_pic", enabled = True)
    objcopy_embed_flags_feature = feature(
        name = "objcopy_embed_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = ["objcopy_embed_data"],
                flag_groups = [flag_group(flags = ["-I", "binary"])],
            )
        ],
    )

    unfiltered_compile_flags_feature = feature(
        name = "unfiltered_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-no-canonical-prefixes",
                            "-fno-canonical-system-headers",
                            "-Wno-builtin-macro-redefined",
                            "-D__DATE__=\"redacted\"",
                            "-D__TIMESTAMP__=\"redacted\"",
                            "-D__TIME__=\"redacted\"",
                        ],
                    ),
                ],
            ),
        ],
    )
    default_compile_flags_feature = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-U_FORTIFY_SOURCE",
                            "-D_FORTIFY_SOURCE=1",
                            "-fstack-protector",
                            "-Wall",
                            "-O2",
                            "-Wunused-but-set-parameter",
                            "-Wno-free-nonheap-object",
                            "-fno-omit-frame-pointer",
                        ],
                    ),
                ],
            ),
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [flag_group(flags = ["-g"])],
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-g0",
                            "-O2",
                            "-DNDEBUG",
                            "-ffunction-sections",
                            "-fdata-sections",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
            flag_set(
                actions = [
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [flag_group(flags = ["-std=c++1z"])],
            ),
        ],
    )
    opt_feature = feature(name = "opt")
    dbg_feature = feature(name = "dbg")
    user_compile_flags_feature = feature(
        name = "user_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = ["%{user_compile_flags}"],
                        iterate_over = "user_compile_flags",
                        expand_if_available = "user_compile_flags",
                    ),
                ],
            ),
        ],
    )
    sysroot_feature = feature(
        name = "sysroot",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                    _CPP_LINK_EXECUTABLE_ACTION_NAME,
                    _CPP_LINK_DYNAMIC_LIBRARY_ACTION_NAME,
                    _CPP_LINK_NODEPS_DYNAMIC_LIBRARY_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                    flags = ["--sysroot=%{sysroot}"],
                    expand_if_available = "sysroot",
                    ),
                ],
            ),
        ],
    )
    objcopy_embed_data_action = action_config(
        action_name = "objcopy_embed_data",
        enabled = True,
        tools = [tool(path = "/usr/bin/objcopy")],
    )
    features = [
        default_compile_flags_feature,
        default_link_flags_feature,
        supports_dynamic_linker_feature,
        supports_pic_feature,
        objcopy_embed_flags_feature,
        opt_feature,
        dbg_feature,
        user_compile_flags_feature,
        sysroot_feature,
        unfiltered_compile_flags_feature,
    ]
    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        features = features,
        cxx_builtin_include_directories = [
            "@PATH_TO_GCC_AARCH94@/../lib/gcc/aarch64-buildroot-linux-gnu/9.4.0/plugin/include",
            "@PATH_TO_GCC_AARCH94@/../lib/gcc/aarch64-buildroot-linux-gnu/9.4.0/install-tools/include",
            "@PATH_TO_GCC_AARCH94@/../lib/gcc/aarch64-buildroot-linux-gnu/9.4.0/include",
            "@PATH_TO_GCC_AARCH94@/../aarch64-buildroot-linux-gnu/sysroot/usr/include",
            "@PATH_TO_GCC_AARCH94@/../aarch64-buildroot-linux-gnu/include",
            "@PATH_TO_GCC_AARCH94@/../include",
            "@PATH_TO_GCC_AARCH94@/../lib/gcc/aarch64-buildroot-linux-gnu/9.4.0/include-fixed",
            "@PATH_TO_GCC_AARCH94@/../lib/glib-2.0/include",
        ],
        toolchain_identifier = "aarch64_gcc94-toolchain",
        host_system_name = "local",
        target_system_name = "local",
        target_cpu = "aarch64",
        target_libc = "unknown",
        compiler = "gcc",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
    )

def _impl_x86(ctx):
    tool_paths = [
        tool_path(
            name = "addr2line",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@addr2line",
        ),
        tool_path(
            name = "ar",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@ar",
        ),
        tool_path(
            name = "as",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@as",
        ),
        tool_path(
            name = "c++",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@c++",
        ),
        tool_path(
            name = "c++filt",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@c++filt",
        ),
        tool_path(
            name = "cpp",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@cpp",
        ),
        tool_path(
            name = "dwp",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@dwp",
        ),
        tool_path(
            name = "elfedit",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@elfedit",
        ),
        tool_path(
            name = "g++",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@g++",
        ),
        tool_path(
            name = "gcc",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@gcc",
        ),
        tool_path(
            name = "gccar",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@gcc-ar",
        ),
        tool_path(
            name = "gcc-nm",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@gcc-nm",
        ),
        tool_path(
            name = "gcc-ranlib",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@gcc-ranlib",
        ),
        tool_path(
            name = "gcov",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@gcov",
        ),
        tool_path(
            name = "gcov-dump",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@gcov-dump",
        ),
        tool_path(
            name = "gcov-tool",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@gcov-tool",
        ),
        tool_path(
            name = "gdb",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@gdb",
        ),
        tool_path(
            name = "gfortran",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@gfortran",
        ),
        tool_path(
            name = "gprof",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@gprof",
        ),
        tool_path(
            name = "ld",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@ld",
        ),
        tool_path(
            name = "ld.bfd",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@ld.bfd",
        ),
        tool_path(
            name = "ld.gold",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@ld.gold",
        ),
        tool_path(
            name = "nm",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@nm",
        ),
        tool_path(
            name = "objcopy",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@objcopy",
        ),
        tool_path(
            name = "objdump",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@objdump",
        ),
        tool_path(
            name = "ranlib",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@ranlib",
        ),
        tool_path(
            name = "readelf",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@readelf",
        ),
        tool_path(
            name = "size",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@size",
        ),
        tool_path(
            name = "strings",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@strings",
        ),
        tool_path(
            name = "strip",
            path = "@PATH_TO_GCC_X86@/@GCC_ARCH_X86@strip",
        ),
    ]

    default_link_flags_feature = feature(
        name = "default_link_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-lstdc++",
                            "-Wl,-z,relro,-z,now",
                            "-no-canonical-prefixes",
                            "-pass-exit-codes",
                        ],
                    ),
                ],
            ),
        flag_set(
            actions = all_link_actions,
            flag_groups = [flag_group(flags = ["-Wl,--gc-sections"])],
            with_features = [with_feature_set(features = ["opt"])],
            ),
    ],
    )

    supports_dynamic_linker_feature = feature(name = "supports_dynamic_linker", enabled = True)
    supports_pic_feature = feature(name = "supports_pic", enabled = True)
    objcopy_embed_flags_feature = feature(
        name = "objcopy_embed_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = ["objcopy_embed_data"],
                flag_groups = [flag_group(flags = ["-I", "binary"])],
            )
        ],
    )

    unfiltered_compile_flags_feature = feature(
        name = "unfiltered_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-no-canonical-prefixes",
                            "-fno-canonical-system-headers",
                            "-Wno-builtin-macro-redefined",
                            "-D__DATE__=\"redacted\"",
                            "-D__TIMESTAMP__=\"redacted\"",
                            "-D__TIME__=\"redacted\"",
                        ],
                    ),
                ],
            ),
        ],
    )
    default_compile_flags_feature = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-U_FORTIFY_SOURCE",
                            "-D_FORTIFY_SOURCE=1",
                            "-fstack-protector",
                            "-Wall",
                            "-O2",
                            "-Wunused-but-set-parameter",
                            "-Wno-free-nonheap-object",
                            "-fno-omit-frame-pointer",
                        ],
                    ),
                ],
            ),
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [flag_group(flags = ["-g"])],
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-g0",
                            "-O2",
                            "-DNDEBUG",
                            "-ffunction-sections",
                            "-fdata-sections",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
            flag_set(
                actions = [
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [flag_group(flags = ["-std=c++1z"])],
            ),
        ],
    )
    opt_feature = feature(name = "opt")
    dbg_feature = feature(name = "dbg")
    user_compile_flags_feature = feature(
        name = "user_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _ASSEMBLE_ACTION_NAME,
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                        flags = ["%{user_compile_flags}"],
                        iterate_over = "user_compile_flags",
                        expand_if_available = "user_compile_flags",
                    ),
                ],
            ),
        ],
    )
    sysroot_feature = feature(
        name = "sysroot",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    _PREPROCESS_ASSEMBLE_ACTION_NAME,
                    _LINKSTAMP_COMPILE_ACTION_NAME,
                    _C_COMPILE_ACTION_NAME,
                    _CPP_COMPILE_ACTION_NAME,
                    _CPP_HEADER_PARSING_ACTION_NAME,
                    _CPP_MODULE_COMPILE_ACTION_NAME,
                    _CPP_MODULE_CODEGEN_ACTION_NAME,
                    _LTO_BACKEND_ACTION_NAME,
                    _CLIF_MATCH_ACTION_NAME,
                    _CPP_LINK_EXECUTABLE_ACTION_NAME,
                    _CPP_LINK_DYNAMIC_LIBRARY_ACTION_NAME,
                    _CPP_LINK_NODEPS_DYNAMIC_LIBRARY_ACTION_NAME,
                ],
                flag_groups = [
                    flag_group(
                    flags = ["--sysroot=%{sysroot}"],
                    expand_if_available = "sysroot",
                    ),
                ],
            ),
        ],
    )
    objcopy_embed_data_action = action_config(
        action_name = "objcopy_embed_data",
        enabled = True,
        tools = [tool(path = "/usr/bin/objcopy")],
    )
    features = [
        default_compile_flags_feature,
        default_link_flags_feature,
        supports_dynamic_linker_feature,
        supports_pic_feature,
        objcopy_embed_flags_feature,
        opt_feature,
        dbg_feature,
        user_compile_flags_feature,
        sysroot_feature,
        unfiltered_compile_flags_feature,
    ]

    action_configs = [objcopy_embed_data_action]
    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        features = features,
        action_configs = action_configs,
        artifact_name_patterns = [],
        cxx_builtin_include_directories = ["/usr/lib/gcc/", "/usr/local/include", "/usr/include"],
        toolchain_identifier = "x86_64-gcc-toolchain",
        host_system_name = "local",
        target_system_name = "local",
        target_cpu = "local",
        target_libc = "local",
        compiler = "compiler",
        abi_version = "local",
        abi_libc_version = "local",
        tool_paths = tool_paths,
        make_variables = [],
        builtin_sysroot = None,
        cc_target_os = "Linux",
    )

####

cc_mastiff_toolchain_config_arm32 = rule(
    implementation = _impl_arm,
    attrs = {},
    provides = [CcToolchainConfigInfo],
)

cc_mastiff_toolchain_config_aarch64_641 = rule(
    implementation = _impl_aarch,
    attrs = { "disable_static_cc_toolchains" : attr.bool()},
    provides = [CcToolchainConfigInfo],
)

cc_mastiff_toolchain_config_aarch64_94 = rule(
    implementation = _impl_aarch94,
    attrs = {},
    provides = [CcToolchainConfigInfo],
)

cc_mastiff_toolchain_config_x86_64 = rule(
    implementation = _impl_x86,
    attrs = { "disable_static_cc_toolchains" : attr.bool()},
    provides = [CcToolchainConfigInfo],
)
