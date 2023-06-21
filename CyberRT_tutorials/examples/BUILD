load("@rules_cc//cc:defs.bzl", "cc_binary")
load("//tools/install:install.bzl", "install")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "talker",
    srcs = ["talker.cc"],
    deps = [
        "//cyber",
        "//CyberRT_tutorials/examples/proto:examples_cc_proto",
    ],
)

cc_binary(
    name = "listener",
    srcs = ["listener.cc"],
    deps = [
        "//cyber",
        "//CyberRT_tutorials/examples/proto:examples_cc_proto",
    ],
)

install(
    name = "install",
    targets = [
        ":talker",
        ":listener",
    ],
    runtime_dest = "CyberRT_tutorials/examples",
    deps = [
        "//CyberRT_tutorials/examples/common_component_example:install",
        "//CyberRT_tutorials/examples/timer_component_example:install",
        "//CyberRT_tutorials/examples/proto:cyber_examples_proto_hdrs",
        "//CyberRT_tutorials/examples/proto:cyber_examples_proto_so",
    ],
)

cc_binary(
    name = "paramserver",
    srcs = ["paramserver.cc"],
    deps = [
        "//cyber",
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
    name = "record",
    srcs = ["record.cc"],
    deps = [
        "//cyber",
        "//cyber/proto:record_cc_proto",
    ],
)

cpplint()
