print("bzl file for showing skills")

# 1, generate file by writing string
def _write_file_by_string_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.write(
        output = out,
        content = "write string:\n\tHello ~ {}!\n".
        format(ctx.attr.usr) +
        "\tSay ~ {}\n".format(ctx.attr.say),
    )
    return [DefaultInfo(files = depset([out]))]

write_file_by_string = rule(
    implementation = _write_file_by_string_impl,
    attrs = {
        "usr": attr.string(),
        "say": attr.string(),
    }
)

# 2, genrule generate file
def gen_file_by_macro(name, usr, say):
    contents = "write by native genrule\n\t%s ~ Hello!\n\t" % usr + say
    native.genrule(
        name = name + "_gen",
        outs = [name + ".txt"],
        cmd = "echo \"%s\" > $@" % contents,
    )

# 3, generate file by template
def _hello_world_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name + ".cc")
    ctx.actions.expand_template(
        output = out,
        template = ctx.file.template,
        substitutions = {
            "@PACKAGE@": ctx.attr.name,
            "@WARNING@": "Don't Change, Gen by Machine!!",
            "@NAME@": ctx.attr.user,
            "@GREETING@": ctx.attr.greeting,
        },
    )
    print("name: ", ctx.attr.user)
    print("greeting: ", ctx.attr.greeting)
    print("file template: ", ctx.file.template)
    print("file out: ", out)
    return [DefaultInfo(files = depset([out]))]

gen_cc_by_template = rule(
    implementation = _hello_world_impl,
    attrs = {
        "user": attr.string(default = "unknown person"),
        "greeting": attr.string(mandatory = True),
        "template": attr.label(
            allow_single_file = [".cc.tpl"],
            mandatory = True,
        ),
    },
)

#5, genrule get git version
def git_version_m(name, out, version):
    native.genrule(
        name = name,
        outs = out,
        cmd = "\
            cd CyberRT_tutorials/ && \
            echo -e \"commit: \\c\" > ../$(@) && \
            git log -1 --pretty=format:%h >> ../$(@) && \
            echo -e \", author: \\c\" >> ../$(@) && \
            git log -1 --pretty=format:%an >> ../$(@) && \
            echo -e \", date: \\c\" >> ../$(@) && \
            git log -1 --pretty=format:%cD >> ../$(@) && \
            echo -e \", branch: \\c\" >> ../$(@) && \
            git symbolic-ref --short -q HEAD >> ../$(@) && \
            cd - && cat $(@) && version=`cat $(@)`"
    )

    #, linkopts, visibility, include_prefix):
##!/bin/bash
#cat > $@ <<"EOF"
#EOF
#'''.format(int(with_gflags)),
#echo -e \", branch: \\c\" >> ../$(@) && \
#git name-rev --name-only HEAD >> ../$(@) && \
def mstf_library(name, hdrs=[], srcs=[], copts=[]):
    native.genrule(
        name = "auto_gen_h",
        outs = ["auto_gen.h"],
        cmd = "\
            cd CyberRT_tutorials/ && \
            echo -e \"#define REPO_INFO \\c\" > ../$(@) && \
            echo -e \"\\\"commit: \\c\" >> ../$(@) && \
            git log -1 --pretty=format:%h >> ../$(@) && \
            echo -e \", author: \\c\" >> ../$(@) && \
            git log -1 --pretty=format:%an >> ../$(@) && \
            echo -e \", date: \\c\" >> ../$(@) && \
            git log -1 --pretty=format:%cD >> ../$(@) && \
            echo -e \"\\\"\" >> ../$(@) \
            "
    )
    native.cc_library(
        name = name,
        hdrs = hdrs + ["auto_gen.h"],
        srcs = srcs,
        copts = copts,
        linkopts = [],
        visibility = ["//visibility:public"],
        include_prefix = "",
    )

print("bzl file for showing bazel")
