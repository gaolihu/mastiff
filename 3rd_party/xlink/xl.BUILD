load("@rules_cc//cc:defs.bzl", "cc_library")

genrule(
    name = "fix_file",
    srcs = [
        #"@mastiff//3rd_party/xlink:BUILD",
    ],
    outs = [
        "src/pc/protocols/usb_host.h"
    ],
    cmd = "touch $@ && \
            sed -i \'1c #ifndef _USB_HOST_H_' external/luxonis_xlink/src/pc/protocols/usb_host.h && \
            sed -i \'2c #define _USB_HOST_H_' external/luxonis_xlink/src/pc/protocols/usb_host.h && \
            sed -i \'81a #endif '  external/luxonis_xlink/src/pc/protocols/usb_host.h",
)

cc_library(
    name = "xlink",
    srcs = glob(
        ["src/shared/*.c"] +
        ["src/shared/*.cpp"] +
        ["src/pc/*.c"] +
        ["src/pc/protocols/*.c"] +
        ["src/pc/protocols/*.cpp"] +
        ["src/pc/*.cpp"]) +
        [":fix_file"],
    hdrs = glob(["include/**/*.h"]),
    includes = [
        "include",
        "include/XLink",
        "src/pc/",
        "src/pc/protocols/",
    ] +
    [":fix_file"],
    deps = [
        "@luxonis_libusb//:lxs_libusb",
    ],
    copts = [
        "-Wno-error=format-truncation",
        "-Wno-error=unused-variable",
        #"-Wno-error=implicit-function-declaration",
        #"-Wno-error=return-type"
        "-DXLINK_ENABLE_LIBUSB",
        "-DUSE_USB_VSC",
        "-Wno-error=sizeof-pointer-memaccess",
        "-Wno-error=unused-function",
        "-Wno-error=unused-const-variable",
    ],
    linkopts = [
    ],
    #strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

