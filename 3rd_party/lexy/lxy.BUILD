package(default_visibility = ["//visibility:public"])

cc_library(
    name = "lxy",
    srcs = [
        "src/input/file.cpp",
    ],
    hdrs = glob([
        "include/**/*.h",
        "include/**/*.hpp",
#"include/lexy/code_point.hpp",
    ]),
    includes = [
        "include",
        "include/lexy",
        "include/lexy_ext",
    ],
    copts = [
        '-DLEXY_HAS_UNICODE_DATABASE=1',
    ],
    visibility = ["//visibility:public"],
)
