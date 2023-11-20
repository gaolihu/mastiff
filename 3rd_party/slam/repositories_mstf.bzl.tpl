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

"""External dependencies for Cartographer."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def cartographer_repositories():
    _maybe(
        http_archive,
        sha256 = "3ef7f5082a035c747f5f0a49d810c722fdff1116b6d853285750ddebd167cc79",
        name = "com_github_nelhage_rules_boost",
        strip_prefix = "rules_boost-f364c077cf94e479359f09cd88dec70d1420474b",
        urls = [
            "@REMOTE_HTTP_SERVER@/com_github_nelhage_rules_boost/f364c077cf94e479359f09cd88dec70d1420474b.zip",
            #"http://172.17.0.2/third_party/com_github_nelhage_rules_boost/f364c077cf94e479359f09cd88dec70d1420474b.zip",
        ],
    )

    _maybe(
        http_archive,
        name = "bazel_skylib",
        url = "@REMOTE_HTTP_SERVER@/bazel_skylib/bazel-skylib-1.4.2.tar.gz",
        #url = "http://172.17.0.2/third_party/bazel_skylib/bazel-skylib-1.4.2.tar.gz",
        sha256 = "66ffd9315665bfaafc96b52278f57c7e2dd09f5ede279ea6d39b2be471e7e3aa",
    )

    _maybe(
        http_archive,
        name = "zlib",
        build_file = "@com_github_nelhage_rules_boost//:zlib.BUILD",
        url = "@REMOTE_HTTP_SERVER@/zlib/zlib-1.2.13.tar.gz",
        #url = "http://172.17.0.2/third_party/zlib/zlib-1.2.13.tar.gz",
        sha256 = "b3a24de97a8fdbc835b9833169501030b8977031bcb54b3b3ac13740f846ab30",
        strip_prefix = "zlib-1.2.13",
    )

    _maybe(
        http_archive,
        name = "org_bzip_bzip2",
        build_file = "@com_github_nelhage_rules_boost//:bzip2.BUILD",
        sha256 = "ab5a03176ee106d3f0fa90e381da478ddae405918153cca248e682cd0c4a2269",
        strip_prefix = "bzip2-1.0.8",
        urls = [
            "@REMOTE_HTTP_SERVER@/org_bzip_bzip2/bzip2-1.0.8.tar.gz",
            #"http://172.17.0.2/third_party/org_bzip_bzip2/bzip2-1.0.8.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "org_lzma_lzma",
        build_file = "@com_github_nelhage_rules_boost//:lzma.BUILD",
        url = "@REMOTE_HTTP_SERVER@/org_lzma_lzma/xz-5.4.4.tar.gz",
        #url = "http://172.17.0.2/third_party/org_lzma_lzma/xz-5.4.4.tar.gz",
        sha256 = "aae39544e254cfd27e942d35a048d592959bd7a79f9a624afb0498bb5613bdf8",
        strip_prefix = "xz-5.4.4",
    )

    _maybe(
        http_archive,
        name = "com_github_facebook_zstd",
        build_file = "@com_github_nelhage_rules_boost//:zstd.BUILD",
        url = "@REMOTE_HTTP_SERVER@/com_github_facebook_zstd/zstd-1.5.5.tar.gz",
        #url = "http://172.17.0.2/third_party/com_github_facebook_zstd/zstd-1.5.5.tar.gz",
        sha256 = "9c4396cc829cfae319a6e2615202e82aad41372073482fce286fac78646d3ee4",
        strip_prefix = "zstd-1.5.5",
    )

    _maybe(
        http_archive,
        name = "boost",
        build_file = "@com_github_nelhage_rules_boost//:boost.BUILD",
        patch_cmds = ["rm -f doc/pdf/BUILD"],
        patch_cmds_win = ["Remove-Item -Force doc/pdf/BUILD"],
        url = "@REMOTE_HTTP_SERVER@/boost/boost-1.82.0.tar.gz",
        #url = "http://172.17.0.2/third_party/boost/boost-1.82.0.tar.gz",
        sha256 = "b62bd839ea6c28265af9a1f68393eda37fab3611425d3b28882d8e424535ec9d",
        strip_prefix = "boost-1.82.0",
    )

    _maybe(
        http_archive,
        name = "openssl",
        url = "@REMOTE_HTTP_SERVER@//openssl/8c3fb405ce789ea820e6eb8c77b13a3b0b351c41.tar.gz",
        #url = "http://172.17.0.2/third_party//openssl/8c3fb405ce789ea820e6eb8c77b13a3b0b351c41.tar.gz",
        sha256 = "0b714175b1697f29b89740187b4b66b3c36c9b9a74a92b648d9a4200869fc160",
        strip_prefix = "boringssl-8c3fb405ce789ea820e6eb8c77b13a3b0b351c41",
    )

    _maybe(
        http_archive,
        name = "com_github_antonovvk_bazel_rules",
        sha256 = "2f5327a2dc9a0cc8ead93953a5d2ae2e0308aece685e46cc89c27538a7e9a73a",
        strip_prefix = "bazel_rules-c76e47ebe6f0a03b9dd99e245d5a0611813c36f9",
        urls = [
            "@REMOTE_HTTP_SERVER@/com_github_antonovvk_bazel_rules/c76e47ebe6f0a03b9dd99e245d5a0611813c36f9.tar.gz",
            #"http://172.17.0.2/third_party/com_github_antonovvk_bazel_rules/c76e47ebe6f0a03b9dd99e245d5a0611813c36f9.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "net_zlib_zlib",
        sha256 = "6d4d6640ca3121620995ee255945161821218752b551a1a180f4215f7d124d45",
        build_file = "//3rd_party/slam/slam_3rd:zlib.BUILD",
        strip_prefix = "zlib-cacf7f1d4e3d44d871b605da3b647f07d718623f",
        urls = [
            "@REMOTE_HTTP_SERVER@/net_zlib_zlib/cacf7f1d4e3d44d871b605da3b647f07d718623f.tar.gz",
            #"http://172.17.0.2/third_party/net_zlib_zlib/cacf7f1d4e3d44d871b605da3b647f07d718623f.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "org_cairographics_pixman",
        build_file = "//3rd_party/slam/slam_3rd/pixman:pixman.BUILD",
        sha256 = "21b6b249b51c6800dc9553b65106e1e37d0e25df942c90531d4c3997aa20a88e",
        strip_prefix = "pixman-0.34.0",
        urls = [
            "@REMOTE_HTTP_SERVER@/org_cairographics_pixman/pixman-0.34.0.tar.gz",
            #"http://172.17.0.2/third_party/org_cairographics_pixman/pixman-0.34.0.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "org_cairographics_cairo",
        build_file = "//3rd_party/slam/slam_3rd/cairo:cairo.BUILD",
        sha256 = "7e87878658f2c9951a14fc64114d4958c0e65ac47530b8ac3078b2ce41b66a09",
        strip_prefix = "cairo-1.14.10",
        urls = [
            "@REMOTE_HTTP_SERVER@/org_cairographics_cairo/cairo-1.14.10.tar.xz",
            "http://172.17.0.2/third_party/org_cairographics_cairo/cairo-1.14.10.tar.xz",
        ],
    )

    _maybe(
        http_archive,
        name = "org_freetype_freetype2",
        build_file = "//3rd_party/slam/slam_3rd:freetype2.BUILD",
        sha256 = "33a28fabac471891d0523033e99c0005b95e5618dc8ffa7fa47f9dadcacb1c9b",
        strip_prefix = "freetype-2.8",
        urls = [
            "@REMOTE_HTTP_SERVER@/org_freetype_freetype2/freetype-2.8.tar.gz",
            #"http://172.17.0.2/third_party/org_freetype_freetype2/freetype-2.8.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "org_libgd_libgd",
        build_file = "//3rd_party/slam/slam_3rd:gd.BUILD",
        sha256 = "a66111c9b4a04e818e9e2a37d7ae8d4aae0939a100a36b0ffb52c706a09074b5",
        strip_prefix = "libgd-2.2.5",
        urls = [
            "@REMOTE_HTTP_SERVER@/org_libgd_libgd/libgd-2.2.5.tar.gz",
            #"http://172.17.0.2/third_party/org_libgd_libgd/libgd-2.2.5.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "org_freedesktop_fontconfig",
        build_file = "//3rd_party/slam/slam_3rd/fontconfig:fontconfig.BUILD",
        sha256 = "fd5a6a663f4c4a00e196523902626654dd0c4a78686cbc6e472f338e50fdf806",
        strip_prefix = "fontconfig-2.12.4",
        urls = [
            "@REMOTE_HTTP_SERVER@/org_freedesktop_fontconfig/fontconfig-2.12.4.tar.gz",
            #"http://172.17.0.2/third_party/org_freedesktop_fontconfig/fontconfig-2.12.4.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "org_ceres_solver_ceres_solver",
        build_file = "//3rd_party/slam/slam_3rd:ceres.BUILD",
        sha256 = "ede5b4205ee8d7c7e029e9da74c7ee759fee6961f7e6bfa694274e4a55b8c7ca",
        strip_prefix = "ceres-solver-58c5edae2f7c4d2533fe8a975c1f5f0b892dfd3e",
        urls = [
            "@REMOTE_HTTP_SERVER@/org_ceres_solver_ceres_solver/58c5edae2f7c4d2533fe8a975c1f5f0b892dfd3e.tar.gz",
            #"http://172.17.0.2/third_party/org_ceres_solver_ceres_solver/58c5edae2f7c4d2533fe8a975c1f5f0b892dfd3e.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "org_tuxfamily_eigen",
        build_file = "//3rd_party/slam/slam_3rd:eigen.BUILD",
        sha256 = "ca7beac153d4059c02c8fc59816c82d54ea47fe58365e8aded4082ded0b820c4",
        strip_prefix = "eigen-eigen-f3a22f35b044",
        urls = [
            "@REMOTE_HTTP_SERVER@/org_tuxfamily_eigen/f3a22f35b044.tar.gz",
            #"http://172.17.0.2/third_party/org_tuxfamily_eigen/f3a22f35b044.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "com_github_libexpat_libexpat",
        build_file = "//3rd_party/slam/slam_3rd:expat.BUILD",
        sha256 = "b5dcb503e40f615a0296a18acc6d975f2f1a3d01c7b3a4b3bb69de27bc9475c3",
        strip_prefix = "libexpat-R_2_2_4/expat",
        urls = [
            "@REMOTE_HTTP_SERVER@/com_github_libexpat_libexpat/R_2_2_4.tar.gz",
            #"http://172.17.0.2/third_party/com_github_libexpat_libexpat/R_2_2_4.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "libjpeg",
        build_file = "//3rd_party/slam/slam_3rd:libjpeg.BUILD",
        sha256 = "240fd398da741669bf3c90366f58452ea59041cacc741a489b99f2f6a0bad052",
        strip_prefix = "jpeg-9b",
        urls = [
            "@REMOTE_HTTP_SERVER@/libjpeg/jpegsrc.v9b.tar.gz",
            #"http://172.17.0.2/third_party/libjpeg/jpegsrc.v9b.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "org_libpng_libpng",
        build_file = "//3rd_party/slam/slam_3rd:libpng.BUILD",
        sha256 = "7f415186d38ca71c23058386d7cf5135c8beda821ee1beecdc2a7a26c0356615",
        strip_prefix = "libpng-1.2.57",
        urls = [
            "@REMOTE_HTTP_SERVER@/org_libpng_libpng/v1.2.57.tar.gz",
            "http://172.17.0.2/third_party/org_libpng_libpng/v1.2.57.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "org_lua_lua",
        build_file = "//3rd_party/slam/slam_3rd:lua.BUILD",
        sha256 = "b9e2e4aad6789b3b63a056d442f7b39f0ecfca3ae0f1fc0ae4e9614401b69f4b",
        strip_prefix = "lua-5.2.4",
        urls = [
            "@REMOTE_HTTP_SERVER@/org_lua_lua/lua-5.2.4.tar.gz",
            "http://172.17.0.2/third_party/org_lua_lua/lua-5.2.4.tar.gz",
        ],
    )

    _maybe(
        http_archive,
        name = "com_github_jupp0r_prometheus_cpp",
        sha256 = "cdf03ee63fcb1d9e113f7bd525e043c254729dddf19d80396489f5b92c83c18d",
        strip_prefix = "prometheus-cpp-b1234816facfdda29845c46696a02998a4af115a",
        # b1234816facfdda29845c46696a02998a4af115a, 2023/8/9
        urls = [
            "@REMOTE_HTTP_SERVER@/com_github_jupp0r_prometheus_cpp/b1234816facfdda29845c46696a02998a4af115a.zip",
            #"http://172.17.0.2/third_party/com_github_jupp0r_prometheus_cpp/b1234816facfdda29845c46696a02998a4af115a.zip",
        ],
    )

    _maybe(
        http_archive,
        name = "com_github_googlecartographer_async_grpc",
        sha256 = "0278c0b35245f04c1f27cabd8ec93f953e19d786d8d3a14741d4793adbf2c40c",
        strip_prefix = "async_grpc-af6e44dd1c18c51bc8608fe937d0b2d376b589d6",
        # af6e44dd1c18c51bc8608fe937d0b2d376b589d6, 2019/2/3
        urls = [
            "@REMOTE_HTTP_SERVER@/com_github_googlecartographer_async_grpc/af6e44dd1c18c51bc8608fe937d0b2d376b589d6.zip",
            #"http://172.17.0.2/third_party/com_github_googlecartographer_async_grpc/af6e44dd1c18c51bc8608fe937d0b2d376b589d6.zip",
        ],
    )

    _maybe(
        http_archive,
        name = "com_google_absl",
        sha256 = "f9021e90a0b7fdec755d33d245b07cab31fef4bad47eeefb937e168a0a6cb579",
        strip_prefix = "abseil-cpp-334aca32051ef6ede2711487acf45d959e9bdffc",
        urls = [
            "@REMOTE_HTTP_SERVER@/com_google_absl/334aca32051ef6ede2711487acf45d959e9bdffc.zip",
            #"http://172.17.0.2/third_party/com_google_absl/334aca32051ef6ede2711487acf45d959e9bdffc.zip",
        ],
    )

    _maybe(
        http_archive,
        name = "rules_python",
        sha256 = "e5470e92a18aa51830db99a4d9c492cc613761d5bdb7131c04bd92b9834380f6",
        strip_prefix = "rules_python-4b84ad270387a7c439ebdccfd530e2339601ef27",
        urls = [
            "@REMOTE_HTTP_SERVER@/rules_python/4b84ad270387a7c439ebdccfd530e2339601ef27.tar.gz",
            #"http://172.17.0.2/third_party/rules_python/4b84ad270387a7c439ebdccfd530e2339601ef27.tar.gz",
        ],
    )

    # TODO(rodrigoq): remove these binds once grpc#14140 has been merged, as well
    # as removing `use_3rd_party` in cartographer_grpc/BUILD.bazel.
    # https://github.com/grpc/grpc/pull/14140
    native.bind(
        name = "grpc_cpp_plugin",
        actual = "@com_github_grpc_grpc//:grpc_cpp_plugin",
    )
    native.bind(
        name = "grpc++_codegen_proto",
        actual = "@com_github_grpc_grpc//:grpc++_codegen_proto",
    )

def _maybe(repo_rule, name, **kwargs):
    if name not in native.existing_rules():
        repo_rule(name = name, **kwargs)
