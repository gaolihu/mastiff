package(default_visibility = ["//visibility:public"])

platform(
    name = "linux_aarch_64",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:aarch64",
        #":glibc_2_25",
    ],
)


cc_library(
    name = "bt",
    srcs = [
        "src/action_node.cpp",
        "src/behavior_tree.cpp",
        "src/blackboard.cpp",
        "src/condition_node.cpp",
        "src/control_node.cpp",
        "src/controls/fallback_node.cpp",
        "src/controls/if_then_else_node.cpp",
        "src/controls/manual_node.cpp",
        "src/controls/parallel_node.cpp",
        "src/controls/reactive_fallback.cpp",
        "src/controls/reactive_sequence.cpp",
        "src/controls/sequence_node.cpp",
        "src/controls/sequence_star_node.cpp",
        "src/controls/switch_node.cpp",
        "src/controls/while_do_else_node.cpp",
        "src/decorator_node.cpp",
        "src/decorators/delay_node.cpp",
        "src/decorators/inverter_node.cpp",
        "src/decorators/repeat_node.cpp",
        "src/decorators/retry_node.cpp",
        "src/decorators/subtree_node.cpp",
#"src/example.cpp",
#"src/loggers/bt_cout_logger.cpp",
#"src/loggers/bt_file_logger.cpp",
#"src/loggers/bt_zmq_publisher.cpp",
#"src/loggers/bt_minitrace_logger.cpp",
        "src/script_parser.cpp",
        "src/shared_library.cpp",
        "src/shared_library_UNIX.cpp",
#"src/shared_library_WIN.cpp",
        "src/tree_node.cpp",
        "src/basic_types.cpp",
        "src/bt_factory.cpp",
        "src/xml_parsing.cpp",
    ],
    hdrs = [
#"src/loggers/zmq.hpp",
    ],
    copts = [
        '-DBT_BOOST_COROUTINE2',
        '-DLEXY_HAS_UNICODE_DATABASE=1',
        '-DBTCPP_LIBRARY_VERSION=\\"4.3\\"',
    ],
    deps = [
        "@boost//:coroutine",
        "@tinyxml2//:txml2",
        "@minitrace//:mt",
        "@lexy//:lxy",
        "@minicoro//:mc",
        "@wildcards//:wc",
    ],
    includes = [
        "include",
        "3rdparty",
    ],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)
