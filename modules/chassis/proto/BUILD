load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_proto_library")

package(default_visibility = ["//visibility:public"])

cc_proto_library(
    name = "chassis_config_cc_proto",
    deps = [
        ":chassis_config_proto",
    ],
)

proto_library(
    name = "chassis_config_proto",
    srcs = [
        "chassis_config.proto",
    ],
    deps = [
        "@com_google_protobuf//:wrappers_proto",
    ],
)

cc_proto_library(
    name = "input_output_chs_cc_proto",
    deps = [
        ":input_output_chs_proto",
    ],
)

proto_library(
    name = "input_output_chs_proto",
    srcs = [
        "input_output_chs.proto",
    ],
    deps = [
        "@com_google_protobuf//:wrappers_proto",
        ":frame_up_stream_proto",
        ":frame_down_stream_proto",
    ],
)

cc_proto_library(
    name = "frame_down_stream_cc_proto",
    deps = [
        ":frame_down_stream_proto",
    ],
)

proto_library(
    name = "frame_down_stream_proto",
    srcs = [
        "frame_down_stream.proto",
    ],
    deps = [
        "@com_google_protobuf//:wrappers_proto",
        ":ys_pack_mcu_protocol_proto",
    ],
)

cc_proto_library(
    name = "ys_pack_mcu_protocol_cc_proto",
    deps = [
        ":ys_pack_mcu_protocol_proto",
    ],
)

proto_library(
    name = "ys_pack_mcu_protocol_proto",
    srcs = [
        "protocol_libs/ys_pack_mcu_protocol.proto",
    ],
)

cc_proto_library(
    name = "frame_up_stream_cc_proto",
    deps = [
        ":frame_up_stream_proto",
    ],
)

proto_library(
    name = "frame_up_stream_proto",
    srcs = [
        "frame_up_stream.proto",
    ],
    deps = [
        "@com_google_protobuf//:wrappers_proto",
    ],
)
