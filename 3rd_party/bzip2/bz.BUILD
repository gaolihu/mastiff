load("@rules_cc//cc:defs.bzl", "cc_library")

genrule(
    name = "fix_file",
    srcs = [
        #"bzlib.c",
    ],
    outs = [
        "bbzlib.c"
    ],
    cmd = "touch $@ && \
            sed -i \'32c  #define BZ_VERSION \"bzip-bazel\"' external/bzip2/bzlib.c",
)

cc_library(
    name = "bz2",
    srcs = glob(["*.c"]) + [":fix_file"],
    hdrs = glob(["*.h"]),
    includes = [
        ".",
    ],
    deps = [
    ],
    copts = [
        "-Wno-error=pointer-sign",
        "-DBZ_UNIX",
        "-Wno-error=unused-result",
    ],
    linkopts = [
    ],
    #strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    linkstatic = 1,
)
