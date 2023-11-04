# Copyright 2018 The Cartographer Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Cartographer is a system that provides real-time simultaneous localization
# and mapping (SLAM) in 2D and 3D across multiple platforms and sensor
# configurations.

##BZL.DEL:this
#BZL.DEL:all

licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//visibility:public"])

exports_files(["LICENSE"])

filegroup(
    name = "configuration_files",
    srcs = glob([
        "configuration_files/*.lua",
    ]),
)

cc_library(
    name = "slam",
    deps = [
        ":map_builder_grpc_server_main",
        ":print_configuration_main",
        ":autogenerate_ground_truth_main",
        ":compute_relations_metrics_main",
        ":pbstream_main",
        "//modules/slam/cartographer_230401/cartographer:cartographer",
    ],
)

##BZL.BUILD:map_builder_grpc_server_main
cc_binary(
    name = "map_builder_grpc_server_main",
    deps = [
        "//modules/slam/cartographer_230401/cartographer/cloud:cartographer_grpc_server_lib",
    ],
)

##BZL.BUILD:print_configuration_main
cc_binary(
    name = "print_configuration_main",
    deps = [
        "//modules/slam/cartographer_230401/cartographer:print_configuration_main_lib",
    ],
)

##BZL.BUILD:autogenerate_ground_truth_main
cc_binary(
    name = "autogenerate_ground_truth_main",
    deps = [
        "//modules/slam/cartographer_230401/cartographer:autogenerate_ground_truth_main_lib",
    ],
)

##BZL.BUILD:compute_relations_metrics_main
cc_binary(
    name = "compute_relations_metrics_main",
    deps = [
        "//modules/slam/cartographer_230401/cartographer:compute_relations_metrics_main_lib",
    ],
)

##BZL.BUILD:pbstream_main
cc_binary(
    name = "pbstream_main",
    deps = [
        "//modules/slam/cartographer_230401/cartographer:pbstream_main_lib",
    ],
)
