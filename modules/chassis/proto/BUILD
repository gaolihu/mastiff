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
    name = "external_msg_cc_proto",
    deps = [
        ":external_msg_proto",
    ],
)

proto_library(
    name = "external_msg_proto",
    srcs = [
        "external_msg.proto",
    ],
    deps = [
        "//modules/aventurier_common_msgs:ros_msgs_proto",
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
    deps = [
        "@com_google_protobuf//:wrappers_proto",
    ],
)

####
cc_proto_library(
    name = "chss_io_cc_proto",
    deps = [
        ":chss_io_proto",
    ],
)

proto_library(
    name = "chss_io_proto",
    srcs = [
        "chss_io.proto",
    ],
    deps = [
        ":internal_msg_proto",
        ":external_msg_proto",
    ],
)

cc_proto_library(
    name = "internal_msg_cc_proto",
    deps = [
        ":internal_msg_proto",
    ],
)

proto_library(
    name = "internal_msg_proto",
    srcs = [
        "internal_msg.proto",
    ],
    deps = [
        "@com_google_protobuf//:wrappers_proto",
    ],
)
