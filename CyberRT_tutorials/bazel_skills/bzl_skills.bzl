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
def git_version(name, **kwargs):
    """Get git ref version of the project"""

    native.genrule(
        name = name,
        cmd = "git rev-parse HEAD",
        **kwargs
    )

print("bzl file for showing bazel")
