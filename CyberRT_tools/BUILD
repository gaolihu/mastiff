load("//tools/install:install.bzl", "install")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "CyberRT_tools",
    deps = [
        #"//CyberRT_tools/cyber_monitor",
        "//CyberRT_tools/cyber_recorder",
    ],
)

install(
    name = "install",
    deps = [
        "//cyber/tools/cyber_launch:install",
        "//cyber/tools/cyber_monitor:install",
        "//cyber/tools/cyber_recorder:install",
        "//cyber/tools/cyber_channel:install",
        "//cyber/tools/cyber_node:install",
        "//cyber/tools/cyber_service:install",
    ],
)
