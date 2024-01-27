load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"],)

#### main target library ####
print("web_nginx build!")
cc_library(
    name = "web_nginx",
    deps = [
        "@fringe_nginx",
    ],
)
