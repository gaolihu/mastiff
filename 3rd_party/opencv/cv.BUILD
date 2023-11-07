load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "opencv4",
    srcs = glob(["lib/*.so*"]),
    hdrs = glob(["include/**/*"]),
    includes = [
        "include/opencv4/",
    ],
    linkopts = [
    ],
    #strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

