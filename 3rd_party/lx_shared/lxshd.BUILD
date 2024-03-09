load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "depthai_shared",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob(["include/**/*.h"]),
    includes = [
        "include/",
        "include/XLink/",
        "3rdparty/"
    ],
    deps = [
        "@libnop//:nop",
        "@com_github_nlohmann_json//:json",
    ],
    linkopts = [
    ],
    #strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    linkstatic = 1,
)

