package(default_visibility = ["//visibility:public"])

cc_library(
    name = "tiny_alsa",
    srcs = glob(["lib/*.so*"]),
    hdrs = glob(["include/**/*.hpp", "include/**/*.h","include/**/**/*.hpp"]),
    includes = ["include"],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)
