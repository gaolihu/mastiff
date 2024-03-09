load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "depthai_shared_bootloader",
    srcs = glob(["src/*.cpp"] + ["src/*.c"]),
    hdrs = glob(["include/**/*.hpp"]),
    includes = [
        "include/",
    ],
    deps = [
    ],
    linkopts = [
    ],
    #strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

