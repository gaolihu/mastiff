load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_proto_library")

package(default_visibility = ["//visibility:public"])

#cambrian config
cc_proto_library(
    name = "cambrian_arch_cc_proto",
    deps = [
        ":cambrian_arch_proto",
    ],
)

proto_library(
    name = "cambrian_arch_proto",
    srcs = [
        "cambrian_arch.proto",
    ],
    deps = [
        ":cambrian_data_proto",
    ],
)

#cambrian data
cc_proto_library(
    name = "cambrian_data_cc_proto",
    deps = [
        ":cambrian_data_proto",
    ],
)

proto_library(
    name = "cambrian_data_proto",
    srcs = [
        "cambrian_data.proto",
    ],
    deps = [
        "@com_google_protobuf//:wrappers_proto",
        "//modules/chassis/proto:input_output_chs_proto",
        "//modules/chassis/proto:external_msg_proto"
    ],
)
