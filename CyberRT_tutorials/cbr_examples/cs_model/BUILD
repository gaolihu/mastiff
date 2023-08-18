load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cs_model",
    deps = [
        "service",
        "client",
    ],
)

cc_binary(
    name = "service",
    srcs = ["service.cc"],
    deps = [
        "//cyber",
        "//CyberRT_tutorials/cbr_examples/proto:cbr_examples_cc_proto",
    ],
)

genrule(
    name = "library_x",
    outs = ["x.cc",],
    cmd = "\
        echo -e \"void x() {return;}\" > $(@) && \
        sed -i \'27c name = \"linux_aarch64\",\' external/boost/BUILD.bazel && \
        sed -i \'35c name = \"linux_x86_64\",\' external/boost/BUILD.bazel && \
        sed -i \'324c \"-msse2\",\' external/boost/BUILD.bazel && \
        sed -i \'325c \"-msse4.1\",\' external/boost/BUILD.bazel && \
        sed -i \'57s|^#||\' external/org_lzma_lzma/BUILD.bazel && \
        rm external/org_lzma_lzma/src/liblzma/api/config.h \
        "
)

cc_library(
    name = "lib_x",
    srcs = ["x.cc"],
    deps = [
        ":library_x",
    ],
)

genrule(
    name = "library_a",
    outs = ["a.cc",],
    cmd = "\
        echo -e \"void x() {return;}\" > $(@) && \
        sed -i \'27c name = \"linux_x86_64\",\' external/boost/BUILD.bazel && \
        sed -i \'35c name = \"linux_aarch64\",\' external/boost/BUILD.bazel && \
        sed -i \'324c #\"-msse2\",\' external/boost/BUILD.bazel && \
        sed -i \'325c #\"-msse4.1\",\' external/boost/BUILD.bazel && \
        sed -i \'57s|^|#|\' external/org_lzma_lzma/BUILD.bazel && \
        cp external/com_github_nelhage_rules_boost/config.lzma-osx-arm64.h external/org_lzma_lzma/src/liblzma/api/config.h \
        "
)

cc_library(
    name = "lib_a",
    srcs = ["a.cc"],
    deps = [
        ":library_a",
        "@org_lzma_lzma//:lzma",
    ],
)

cc_binary(
    name = "client",
    srcs = ["client.cc"],
    deps = [
        ":lib_a",
        "//cyber",
        "//CyberRT_tutorials/cbr_examples/proto:cbr_examples_cc_proto",
        "@behaviour_tree//:bt",
    ],
)

cpplint()
