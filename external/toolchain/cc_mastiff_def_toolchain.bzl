# 1, masstiff library
def _mstf_library_impl(ctx):
    mstf_toolchain = ctx.toolchains["//external/toolchain:mstf_toolchain_type"]
    print("\n" + "\n".join([
        "mstf_library(",
        "  name = '" + ctx.attr.name + "',",
        "  toolchain = {",
        "    'targetting_cpu': '" + mstf_toolchain.targetting_cpu + "',",
        "    'targetting_os': '" + mstf_toolchain.targetting_os + "',",
        "    'executing_on_cpu': '" + mstf_toolchain.executing_on_cpu + "',",
        "    'executing_on_os': '" + mstf_toolchain.executing_on_os + "',",
        "  },",
        ")",
    ]))
    return []

mstf_library = rule(
    implementation = _mstf_library_impl,
    toolchains = ["//external/toolchain:mstf_toolchain_type"],
)

# 2, masstiff toolchain
def _mstf_toolchain_impl(ctx):
    toolchain_info = platform_common.ToolchainInfo(
        targetting_cpu = ctx.attr.targetting_cpu,
        targetting_os = ctx.attr.targetting_os,
        executing_on_cpu = ctx.attr.executing_on_cpu,
        executing_on_os = ctx.attr.executing_on_os,
    )
    return [toolchain_info]

mstf_toolchain = rule(
    implementation = _mstf_toolchain_impl,
    attrs = {
        "targetting_cpu": attr.string(mandatory = True),
        "targetting_os": attr.string(mandatory = True),
        "executing_on_cpu": attr.string(mandatory = True),
        "executing_on_os": attr.string(mandatory = True),
        #"compiler_path": attr.string(),
        #"system_lib": attr.string(),
        #"arch_flags": attr.string_list(),
    },
)

# 3, fail msgs
def _fail_with_msg(ctx):
    mstf_toolchain = ctx.toolchains["//external/toolchain:mstf_toolchain_type"]
    fail(ctx.attr.msg + " Selected toolchain: " + str(mstf_toolchain) + ".")

fail_with_msg = rule(
    implementation = _fail_with_msg,
    attrs = {
        "msg": attr.string(mandatory = True),
    },
    toolchains = ["//external/toolchain:mstf_toolchain_type"],
)

"""
####
TOOLCHAIN_SUPPORT_MATRIX = {
    "hisi": {
        TOOLCHAIN_HOST_OS : "linux",
        TOOLCHAIN_TARGET_OS : "linux",
        TOOLCHAIN_TARGET_ARCH : "armv7",
        TOOLCHAIN_COMPILER_ROOT : "",
        TOOLCHAIN_INCLUDE_PATHS : [],
        TOOLCHAIN_IDENTIFIER : "",
        TOOLCHAIN_CC_COMPILER : "gcc"
    },
    "ubuntu_gcc": {
        TOOLCHAIN_HOST_OS : "linux",
        TOOLCHAIN_TARGET_OS : "linux",
        TOOLCHAIN_TARGET_ARCH : "x86-64",
        TOOLCHAIN_COMPILER_ROOT : "/usr/bin/",
        TOOLCHAIN_INCLUDE_PATHS : [
            "/usr/include",
            "/usr/lib/gcc",
            "/usr/local/include"
        ],
        TOOLCHAIN_IDENTIFIER : "",
        TOOLCHAIN_CC_COMPILER : "gcc"
    },
    "ubuntu_clang": {
        TOOLCHAIN_HOST_OS : "linux",
        TOOLCHAIN_TARGET_OS : "linux",
        TOOLCHAIN_TARGET_ARCH : "x86-64",
        TOOLCHAIN_COMPILER_ROOT : "",
        TOOLCHAIN_INCLUDE_PATHS : [],
        TOOLCHAIN_IDENTIFIER : "",
        TOOLCHAIN_CC_COMPILER : "clang"
    },
    "ubuntu_arm_linux_gnueabihf" : {
        TOOLCHAIN_HOST_OS : "linux",
        TOOLCHAIN_TARGET_OS : "linux",
        TOOLCHAIN_TARGET_ARCH : "aarch64",
        TOOLCHAIN_COMPILER_ROOT : "/usr/bin/",
        TOOLCHAIN_INCLUDE_PATHS : [
            "/usr/arm-linux-gnueabihf/include/",
            "/usr/lib/gcc-cross/arm-linux-gnueabihf/7/include",
        ],
        TOOLCHAIN_IDENTIFIER : "arm-linux-gnueabihf-",
        TOOLCHAIN_CC_COMPILER : "gcc"
    }
}

def _file_tpl_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.expand_template(
        output = out,
        template = ctx.file.template,
        substitutions = {"{NAME}": ctx.attr.username},
    )
    return [DefaultInfo(files = depset([out]))]

gen_cc_toolchain_config = rule(
    implementation = _file_tpl_impl,
    attrs = {
        "username": attr.string(default = "unknown person"),
        "template": attr.label(
            allow_single_file = [".tpl"],
            mandatory = True,
        ),
    },
)

attrs = {
    "include_paths" : attr.string_list(doc = "The compiler include directories."),
    "compiler_root" : attr.string(doc = "The compiler root directory."),
    "host_os" : attr.string(default = "linux", doc = "The cross toolchain prefix."),
    "toolchain_identifier": attr.string(),
    "target_os" : attr.string(default = "linux"),
    "target_arch" : attr.string(default = "x86-64"),
    "cc_compiler" : attr.string(default = "gcc", doc = "The compiler type."),
    "extra_features": attr.string_list(),
}

def generate_toolchain_suite():
    toolchains = {}
    native.filegroup(name = "empty")

    for (platform, toolchain_info) in TOOLCHAIN_SUPPORT_MATRIX.items():
        host_os = toolchain_info[TOOLCHAIN_HOST_OS]
        target_os = toolchain_info[TOOLCHAIN_TARGET_OS]
        target_arch = toolchain_info[TOOLCHAIN_TARGET_ARCH]
        compiler_root = toolchain_info[TOOLCHAIN_COMPILER_ROOT]
        include_paths = toolchain_info[TOOLCHAIN_INCLUDE_PATHS]
        toolchain_identifier = toolchain_info[TOOLCHAIN_IDENTIFIER]
        cc_compiler = toolchain_info[TOOLCHAIN_CC_COMPILER]

        base_name = "{platform}_{target_os}_{target_arch}_{cc_compiler}_{toolchain_identifier}".format(
            platform = platform,
            target_os = target_os,
            target_arch = target_arch,
            cc_compiler = cc_compiler,
            toolchain_identifier = toolchain_identifier
        )

        configuration_name = "%s_cc_toolchain_config" % base_name
        cc_name = "%s_cc_toolchain" % base_name
        toolchain_name = "%s_cc" % base_name

        my_cc_toolchain_config(
            name = configuration_name,
            include_paths = include_paths,
            compiler_root = compiler_root,
            host_os = host_os,
            toolchain_identifier = toolchain_identifier,
            target_os = target_os,
            target_arch = target_arch,
            cc_compiler = cc_compiler,
            extra_features = [],
        )

        cc_toolchain(
            name = cc_name,
            toolchain_identifier = toolchain_name,
            toolchain_config = ":%s" % configuration_name,
            all_files = ":empty",
            compiler_files = ":empty",
            dwp_files = ":empty",
            linker_files = ":empty",
            objcopy_files = ":empty",
            strip_files = ":empty",
            supports_param_files = 0,
        )

        if platform in toolchains.keys():
            print("%s already exist!" % platform)
            fail("%s already exist!" % platform)
        else:
            toolchains[platform] = cc_name

        print("toolchains = ", toolchains)
        cc_toolchain_suite(
            name = "compiler_suite",
            toolchains = toolchains
        )
        """
