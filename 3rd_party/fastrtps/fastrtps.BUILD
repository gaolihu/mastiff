load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "fastrtps",
    includes = [
        "include",
    ],
    linkopts = [
        "-L@FASTRTPS_LIB@",
        "-lfastrtps",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@fastcdr",
    ],
)
