load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "cyber_croutine_hdrs",
    srcs = glob([
        "*.h",
        "detail/*.h"
    ]),
)

cc_library(
    name = "croutine",
    srcs = ["croutine.cc"],
    hdrs = ["croutine.h"],
    linkopts = ["-latomic"],
    deps = [
        "//cyber/base:atomic_hash_map",
        "//cyber/base:atomic_rw_lock",
        "//cyber/base:bounded_queue",
        "//cyber/base:concurrent_object_pool",
        "//cyber/base:macros",
        "//cyber/base:wait_strategy",
        "//cyber/common",
        "//cyber/croutine:routine_context",
        "//cyber/croutine:routine_factory",
        "//cyber/croutine:swap",
        "//cyber/event:perf_event_cache",
        "//cyber/time",
    ],
)

cc_library(
    name = "routine_context",
    srcs = ["detail/routine_context.cc"],
    hdrs = ["detail/routine_context.h"],
    deps = [
        "//cyber/common",
    ],
)

cc_library(
    name = "routine_factory",
    hdrs = ["routine_factory.h"],
    deps = [
        "//cyber/common",
        "//cyber/event:perf_event_cache",
    ],
)

config_setting(
    name = "x86_64",
    values = { "cpu": "x86_64" }
)

config_setting(
    name = "aarch64",
    values = { "cpu": "aarch64" }
)

config_setting(
    name = "aarch64_93",
    values = { "cpu": "aarch64_93" }
)

config_setting(
    name = "aarch64_94",
    values = { "cpu": "aarch64_94" }
)

config_setting(
    name = "aarch64_103",
    values = { "cpu": "aarch64_103" }
)

config_setting(
    name = "aarch64_113",
    values = { "cpu": "aarch64_113" }
)

cc_library(
    name = "swap",
    srcs = select(
        {
            "x86_64": [ "detail/swap_x86_64.S", ],
            "aarch64": [ "detail/swap_aarch64.S", ],
            "aarch64_93": [ "detail/swap_aarch64.S", ],
            "aarch64_94": [ "detail/swap_aarch64.S", ],
            "aarch64_103": [ "detail/swap_aarch64.S", ],
            "aarch64_113": [ "detail/swap_aarch64.S", ],
            "//conditions:default": [ "detail/swap_x86_64.S", ],
        },
        no_match_error = "Please Build with a Linux aarch64 or x86_64 platform",
    ),
)

cc_test(
    name = "croutine_test",
    size = "small",
    srcs = ["croutine_test.cc"],
    deps = [
        "//cyber",
        "@com_google_googletest//:gtest_main",
    ],
    linkstatic = True,
)

cpplint()
