package(default_visibility = ["//visibility:public"],)

### for aarch64 ###
genrule(
    name = "build_nginx_aarch64",
    srcs = [
        "@//3rd_party/fringe_nginx:aarch64_build_zlib.sh",
        "@//3rd_party/fringe_nginx:aarch64_build_pcre.sh",
        "@//3rd_party/fringe_nginx:aarch64_build_openssl.sh",
        "@//3rd_party/fringe_nginx:aarch64_build_http_flv.sh",
        "@//3rd_party/fringe_nginx:aarch64_build_nginx.sh",
    ],
    outs = [
        "hello-nginx-aarch64.c",
    ],
    cmd = "\
        echo \"int main()\n{\n\treturn 0;\n}\" > $(OUTS) && \
        cp $(location @//3rd_party/fringe_nginx:aarch64_build_zlib.sh) 3rd_party/fringe_nginx/ && \
        cp $(location @//3rd_party/fringe_nginx:aarch64_build_pcre.sh) 3rd_party/fringe_nginx/ && \
        cp $(location @//3rd_party/fringe_nginx:aarch64_build_openssl.sh) 3rd_party/fringe_nginx/ && \
        cp $(location @//3rd_party/fringe_nginx:aarch64_build_http_flv.sh) 3rd_party/fringe_nginx/ && \
        cp $(location @//3rd_party/fringe_nginx:aarch64_build_nginx.sh) 3rd_party/fringe_nginx/ && \
        ls 3rd_party/fringe_nginx/ && \
        cd 3rd_party/fringe_nginx/ && \
        mkdir build_aarch64 && \
        chmod a+x aarch64_build_zlib.sh&& \
        chmod a+x aarch64_build_pcre.sh && \
        chmod a+x aarch64_build_openssl.sh && \
        chmod a+x aarch64_build_http_flv.sh && \
        chmod a+x aarch64_build_nginx.sh && \
        ./aarch64_build_zlib.sh && \
        ./aarch64_build_pcre.sh && \
        ./aarch64_build_openssl.sh && \
        ./aarch64_build_http_flv.sh && \
        ./aarch64_build_nginx.sh && \
        echo Build Nginx for aarch64 OK! && \
        ls build_aarch64 && \
        tree build_aarch64 && \
        pwd \
    ",
)

genrule(
    name = "link_nginx_aarch64",
    srcs = [
    ],
    outs = [
        "nginx-locate-aarch64.H",
    ],
    cmd = "\
        realpath build_aarch64 > $(OUTS) \
        cd 3rd_party/fringe_nginx/ && \
        realpath 3rd_party/fringe_nginx/build_aarch64 > $(OUTS) \
    ",
)
### for aarch64 ###

### for x64 ###
genrule(
    name = "build_nginx_x64",
    srcs = [
        "@//3rd_party/fringe_nginx:x64_build_zlib.sh",
        "@//3rd_party/fringe_nginx:x64_build_pcre.sh",
        "@//3rd_party/fringe_nginx:x64_build_openssl.sh",
        "@//3rd_party/fringe_nginx:x64_build_http_flv.sh",
        "@//3rd_party/fringe_nginx:x64_build_nginx.sh",
    ],
    outs = [
        "hello-nginx-x64.c",
    ],
    cmd = "\
        echo \"int main()\n{\n\treturn 0;\n}\" > $(OUTS) && \
        cp $(location @//3rd_party/fringe_nginx:x64_build_zlib.sh) 3rd_party/fringe_nginx/ && \
        cp $(location @//3rd_party/fringe_nginx:x64_build_pcre.sh) 3rd_party/fringe_nginx/ && \
        cp $(location @//3rd_party/fringe_nginx:x64_build_openssl.sh) 3rd_party/fringe_nginx/ && \
        cp $(location @//3rd_party/fringe_nginx:x64_build_http_flv.sh) 3rd_party/fringe_nginx/ && \
        cp $(location @//3rd_party/fringe_nginx:x64_build_nginx.sh) 3rd_party/fringe_nginx/ && \
        ls 3rd_party/fringe_nginx/ && \
        cd 3rd_party/fringe_nginx/ && \
        mkdir build_x64 && \
        chmod a+x x64_build_zlib.sh && \
        chmod a+x x64_build_pcre.sh && \
        chmod a+x x64_build_openssl.sh && \
        chmod a+x x64_build_http_flv.sh && \
        chmod a+x x64_build_nginx.sh && \
        ./x64_build_zlib.sh && \
        ./x64_build_pcre.sh && \
        ./x64_build_openssl.sh && \
        ./x64_build_http_flv.sh && \
        ./x64_build_nginx.sh && \
        echo Build Nginx for x64 OK! && \
        tree build_x64 && \
        pwd \
    ",
)

genrule(
    name = "link_nginx_x64",
    srcs = [
    ],
    outs = [
        "nginx-locate-x64.H",
    ],
    cmd = "\
        realpath build_x64 > $(OUTS) \
        cd 3rd_party/fringe_nginx/ && \
        realpath 3rd_party/fringe_nginx/build_x64 > $(OUTS) \
    ",
)
### for x64 ###

###  ###
cc_library(
    name = "fringe_nginx_x64",
    srcs = [
        "hello-nginx-x64.c",
        "nginx-locate-x64.H",
    ],
    deps = [
    ],
)

cc_library(
    name = "fringe_nginx_aarch64",
    srcs = [
        "hello-nginx-aarch64.c",
        "nginx-locate-aarch64.H",
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
    name = "fringe_nginx",
    deps = select(
        {
            "x86_64": [":fringe_nginx_x64"],
            "aarch64": [":fringe_nginx_aarch64"],
            "aarch64_94": [":fringe_nginx_aarch64"],
            "aarch64_93": [":fringe_nginx_aarch64"],
            "aarch64_103": [":fringe_nginx_aarch64"],
            "aarch64_113": [":fringe_nginx_aarch64"],
        },
        no_match_error = "Please use Linux aarch64 or x86_64 platform",
    ),
)
