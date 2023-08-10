licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//visibility:public"])

exports_files(["LICENSE"])

filegroup(
    name = "configuration_files",
    srcs = glob([
        "configuration_files/*.lua",
    ]),
)

# main library #
cc_library(
    name = "cartographer_230401",
    deps = [
        "//srcs/slam/cartographer_230401/cartographer:map_builder_grpc_server_main",
        "//srcs/slam/cartographer_230401/cartographer:print_configuration_main",
        "//srcs/slam/cartographer_230401/cartographer:autogenerate_ground_truth_main",
        "//srcs/slam/cartographer_230401/cartographer:compute_relations_metrics_main",
        "//srcs/slam/cartographer_230401/cartographer:pbstream_main",
        "//srcs/slam/cartographer_230401/cartographer:cartographer",
    ],
)
