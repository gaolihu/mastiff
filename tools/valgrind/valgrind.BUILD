package(default_visibility = ["//visibility:public"],)

### for aarch64 ###
genrule(
    name = "build_valgrind_aarch64",
    srcs = [
        "@//tools/valgrind:aarch64_build_valgrind.sh",
    ],
    outs = [
        "hello-valgrind-aarch64.c",
    ],
    cmd = "\
        echo \"int main()\n{\n\treturn 0;\n}\" > $(OUTS) && \
        cp $(location @//tools/valgrind:aarch64_build_valgrind.sh) external/valgrind_3_19_0/ && \
        cd external/valgrind_3_19_0/ && \
        chmod a+x aarch64_build_valgrind.sh && \
        ./aarch64_build_valgrind.sh && \
        echo Build valgrind for aarch64 OK! && \
        tree -ls build_aarch64/install/bin/ && \
        pwd \
    ",
)

genrule(
    name = "link_valgrind_aarch64",
    srcs = [
    ],
    outs = [
        "valgrind-locate-aarch64.H",
    ],
    cmd = "\
        realpath build_aarch64 > $(OUTS) \
        cd external/valgrind_3_19_0/ && \
        realpath external/valgrind_3_19_0/build_aarch64/> $(OUTS) \
    ",
)

### for x64 ###
genrule(
    name = "build_valgrind_x64",
    srcs = [
        "@//tools/valgrind:x64_build_valgrind.sh",
    ],
    outs = [
        "hello-valgrind-x64.c",
    ],
    cmd = "\
        echo \"int main()\n{\n\treturn 0;\n}\" > $(OUTS) && \
        cp $(location @//tools/valgrind:x64_build_valgrind.sh) external/valgrind_3_19_0/ && \
        cd external/valgrind_3_19_0/ && \
        chmod a+x x64_build_valgrind.sh && \
        ./x64_build_valgrind.sh && \
        echo Build valgrind for x64 OK! && \
        tree -ls build_x64/install/bin/ && \
        pwd \
    ",
)

genrule(
    name = "link_valgrind_x64",
    srcs = [
    ],
    outs = [
        "valgrind-locate-x64.H",
    ],
    cmd = "\
        realpath build_x64 > $(OUTS) \
        cd external/valgrind_3_19_0/ && \
        realpath external/valgrind_3_19_0/build_x64/ > $(OUTS) \
    ",
)

###  ###
cc_library(
    name = "valgrind_x64",
    srcs = [
        "hello-valgrind-x64.c",
        "valgrind-locate-x64.H",
    ],
    deps = [
    ],
)

cc_library(
    name = "valgrind_aarch64",
    srcs = [
        "hello-valgrind-aarch64.c",
        "valgrind-locate-aarch64.H",
    ],
    deps = [
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

#### main target library ####
cc_library(
    name = "valgrind_3_19_0",
    deps = select(
        {
            "x86_64": [":valgrind_x64"],
            "aarch64": [":valgrind_aarch64"],
            "aarch64_93": [":valgrind_aarch64"],
            "aarch64_94": [":valgrind_aarch64"],
            "aarch64_103": [":valgrind_aarch64"],
            "aarch64_113": [":valgrind_aarch64"],
        },
        no_match_error = "Please use Linux aarch64 or x86_64 platform",
    ),
)
