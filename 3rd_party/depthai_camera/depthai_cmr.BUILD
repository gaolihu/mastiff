load("@rules_cc//cc:defs.bzl", "cc_library")

genrule(
    name = "config_h",
    srcs = [
        "@mastiff//3rd_party/depthai_camera:config.hpp",
    ],
    outs = [
        "include/depthai/build/config.hpp",
    ],
    cmd = "mkdir -p include/depthai/build && \
            cp $< $@",
)

genrule(
    name = "version_h",
    srcs = [
        "@mastiff//3rd_party/depthai_camera:version.hpp",
    ],
    outs = [
        "include/depthai/build/version.hpp",
    ],
    cmd = "mkdir -p include/depthai/build && \
            cp $< $@",
)

genrule(
    name = "cmrc_h",
    srcs = [
        "@mastiff//3rd_party/depthai_camera:cmrc.hpp",
    ],
    outs = [
        "include/cmrc/cmrc.hpp",
    ],
    cmd = "mkdir -p include/cmrc/ && cp $< $@ && \
            sed -i \'14c #define DEPTHAI_HAVE_OPENCV_SUPPORT' external/luxonis_depthai/include/depthai/pipeline/datatype/ImgFrame.hpp",
)

genrule(
    name = "cmrc_c",
    srcs = [
        "@mastiff//3rd_party/depthai_camera:lib_cmrc.cpp",
    ],
    outs = [
        "include/cmrc/lib_cmrc.cpp",
    ],
    cmd = "cp $< $@",
)

genrule(
    name = "cmrc_incbin_h",
    srcs = [
        "@mastiff//3rd_party/depthai_camera:incbin.h",
    ],
    outs = [
        "include/incbin/incbin.h",
    ],
    cmd = "mkdir -p include/incbin/ && cp $< $@",
)

genrule(
    name = "cmrc_fw_c",
    srcs = [
        "@mastiff//3rd_party/depthai_camera:depthai-device-fwp-a95f582a61ec9bdbd0f72dec84822455872ffaf7.tar.xz.cpp",
    ],
    outs = [
        "include/cmrc/depthai-device-fwp-a95f582a61ec9bdbd0f72dec84822455872ffaf7.tar.xz.cpp",
    ],
    cmd = "cp $< $@",
)

genrule(
    name = "cmrc_bl_fw_c",
    srcs = [
        "@mastiff//3rd_party/depthai_camera:depthai-bootloader-fwp-0.0.26.tar.xz.cpp",
    ],
    outs = [
        "include/cmrc/depthai-bootloader-fwp-0.0.26.tar.xz.cpp",
    ],
    cmd = "cp $< $@",
)

cc_library(
    name = "depthai",
    srcs = glob(
        ["src/**/*.c"] +
        ["src/**/*.cpp"]) +
        [":config_h"] +
        [":version_h"] +
        [":cmrc_h"] +
        [":cmrc_incbin_h"] +
        [":cmrc_fw_c"] +
        [":cmrc_bl_fw_c"] +
        [":cmrc_c"],
    hdrs = glob(["include/**/*.h"] + ["include/*"]),
    includes = [
        "include/",
        "include/depthai/",
        "src/",
    ],
    deps = [
        "@luxonis_xlink//:xlink",
        "@luxonis_shared//:depthai_shared",
        "@luxonis_shared_bootloader//:depthai_shared_bootloader",
        "@spdlog//:spdl",
        "@luxonis_fp16//:fp16",
        #"@luxonis_archive//:lxs_archive",
        "@luxonis_archive_binary//:lxs_archive_binary",
        "@opencv4_8//:opencv4",
        "@backward_cpp//:backward",
        "@bzip2//:bz2",
    ],
    copts = [
        "-DDEPTHAI_BOOTLOADER_VERSION=\\\"0.0.26\\\"",
        "-DDEPTHAI_DEVICE_VERSION=\\\"a95f582a61ec9bdbd0f72dec84822455872ffaf7\\\"",
        "-DDEPTHAI_ENABLE_BACKWARD",
        "-DDEPTHAI_ENABLE_LIBUSB",
        "-DDEPTHAI_PATCH_ONLY_MODE",
        "-DDEPTHAI_RESOURCE_COMPILED_BINARIES",
        "-DDEPTHAI_HAVE_OPENCV_SUPPORT",
        "-DDEPTHAI_TARGET_CORE",
    ],
    linkopts = [
    ],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

