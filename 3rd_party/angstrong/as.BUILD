load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "angstrong",
    srcs = glob(["lib/aarch64-linux-gnu/*.so*"]),
    hdrs = glob(["include/**/*"]),
    includes = [
        "include",
    ],
    linkopts = [
    ],
    #strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

