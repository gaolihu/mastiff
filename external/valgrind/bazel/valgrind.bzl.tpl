"""External dependencies for valgrind."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def valgrind_build():
    _maybe(
        http_archive,
        name = "valgrind_3_19_0",
        build_file = "@//tools/valgrind:valgrind.BUILD",
        sha256 = "dd5e34486f1a483ff7be7300cc16b4d6b24690987877c3278d797534d6738f02",
        strip_prefix = "valgrind-3.19.0",
        urls = [
            "@REMOTE_HTTP_SERVER@/valgrind/valgrind-3.19.0.tar.bz2",
        ],
    )

def _maybe(repo_rule, name, **kwargs):
    if name not in native.existing_rules():
        repo_rule(name = name, **kwargs)
