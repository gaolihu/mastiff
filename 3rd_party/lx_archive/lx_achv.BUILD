load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "lxs_archive",
    srcs = glob(["libarchive/*.c"]),
    hdrs = glob(["libarchive/**/*.h"]),
    includes = [
        "libarchive/",
    ],
    deps = [
    ],
    linkopts = [
    ],
    #strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

cc_library(
    name = "lxs_archive_binary",
    srcs = glob(["lib/libarchive.a"]),
    hdrs = glob(["include/*.h"]),
    includes = [
        "include/",
    ],
    deps = [
    ],
    linkopts = [
    ],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)
