load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "fastcdr",
    includes = [
        "include",
    ],
    linkopts = [
        "-L@FASTRTPS_LIB@",
        "-lfastcdr",
    ],
    visibility = ["//visibility:public"],
)
