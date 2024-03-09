load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "spdl",
    srcs = glob(["src/*.cpp"]),
    hdrs = glob(["include/**/*.h"]),
    includes = [
        "include",
    ],
    copts = [
        "-DSPDLOG_COMPILED_LIB",
    ],
    linkopts = [
    ],
    #strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

