load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

##BZL.DEL:this
##BZL.DEL:all

cc_library(
    name = "sp_model",
    deps = [
        "talker",
        "listener",
    ]
)

cc_binary(
    name = "talker",
    srcs = ["talker.cc"],
    deps = [
        "//cyber",
        "//CyberRT_tutorials/cbr_examples/proto:cbr_examples_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
    ],
)

cc_binary(
    name = "listener",
    srcs = ["listener.cc"],
    deps = [
        "//cyber",
        "//CyberRT_tutorials/cbr_examples/proto:cbr_examples_cc_proto",
        "//modules/chassis/proto:input_output_chs_cc_proto",
    ],
)

cpplint()
