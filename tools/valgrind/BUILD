package(
    default_visibility = ["//visibility:public"],
)

exports_files(
    [
        "aarch64_build_valgrind.sh",
        "x64_build_valgrind.sh",
    ]
)

cc_library(
    name = "valgrind",
    deps = [
        "@valgrind_3_19_0",
    ],
)
