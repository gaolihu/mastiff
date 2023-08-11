"""External dependencies for fringe nginx."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def nginx_repo():
    _maybe(
        http_archive,
        name = "fringe_nginx",
        build_file = "@//3rd_party/fringe_nginx:fringe_nginx.BUILD",
        sha256 = "4a49f981d4c4122f16fa8f13f16db8338f67c6731a5422d6f5690be92bd0cfd6",
        strip_prefix = "nginx_for_fringe",
        urls = [
            "@REMOTE_HTTP_SERVER@/nginx/nginx_source_raw_balls.tar",
        ],
    )

def _maybe(repo_rule, name, **kwargs):
    if name not in native.existing_rules():
        repo_rule(name = name, **kwargs)
