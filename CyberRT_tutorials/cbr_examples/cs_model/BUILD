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
        "//CyberRT_tutorials/examples/proto:examples_cc_proto",
    ],
)

cc_binary(
    name = "client",
    srcs = ["client.cc"],
    deps = [
        "//cyber",
        "//CyberRT_tutorials/examples/proto:examples_cc_proto",
        "@behaviour_tree//:bt",
    ],
)

cpplint()
