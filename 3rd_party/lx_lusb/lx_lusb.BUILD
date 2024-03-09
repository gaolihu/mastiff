load("@rules_cc//cc:defs.bzl", "cc_library")

genrule(
    name = "fix_file",
    srcs = [
    ],
    outs = [
        "libusb/libusb-1.0/libusb.h",
    ],
    cmd = "sed -i \'27c //FK' external/luxonis_libusb/libusb/libusbi.h && \
            sed -i \'129a #define DEFAULT_VISIBILITY __attribute__ ((visibility (\"default\")))' external/luxonis_libusb/libusb/libusbi.h && \
            mkdir -p /libusb/libusb-1.0 && \
            cp external/luxonis_libusb/libusb/libusb.h $@",
)

cc_library(
    name = "lxs_libusb",
    srcs = glob(["libusb/*.c"] + ["libusb/os/*posix*.c"]) + [":fix_file"],
    hdrs = glob(["libusb/**/*.h"]),
    includes = [
        "libusb/",
        "libusb/os/",
    ],
    deps = [
    ],
    copts = [
        "-Wno-error=unused-variable",
        "-Wno-error=unused-function",
        "-DPLATFORM_POSIX",
    ],
    linkopts = [
    ],
    #strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

