# 1, generate file by writing string
def _write_file_by_string_impl(ctx):
    #pass
    print("ananlyzing", ctx.label)
    print("ananlyzing", ctx.label.name)

    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.write(
        output = out,
        content = "write string:\n\tHello {}!\n".
        format(ctx.attr.usernam) +
        "\t{}\n".format(ctx.attr.say),
    )
    return [DefaultInfo(files = depset([out]))]

write_file_by_string = rule(
    implementation = _write_file_by_string_impl,
    attrs = {
        "usernam": attr.string(),
        "say": attr.string(),
    }
)

# 2, generate file by template
def _hello_world_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name + ".cc")
    ctx.actions.expand_template(
        output = out,
        template = ctx.file.template,
        substitutions = {
            "{NAME}": ctx.attr.user,
            "{VERSION}": ctx.attr.version,
        },
    )
    print("name: ", ctx.attr.user)
    print("version: ", ctx.attr.version)
    return [DefaultInfo(files = depset([out]))]

gen_file_by_micro_template = rule(
    implementation = _hello_world_impl,
    attrs = {
        "user": attr.string(default = "unknown person"),
        "version": attr.string(mandatory = True),
        "template": attr.label(
            allow_single_file = [".cc.tpl"],
            mandatory = True,
        ),
    },
)

#4, genrule generate file
def gen_file_by_macro(name, content):
    """Generate file by bzl macro"""
    content = "Hello %s!\n" % name + content

    native.genrule(
        name = name + "_gen",
        outs = [name + ".txt"],
        cmd = "echo \"%s\" > $@" % content,
    )

#5, genrule get git version
def git_version(name, **kwargs):
    """Get git ref version of the project"""

    native.genrule(
        name = name,
        cmd = "git rev-parse HEAD",
        **kwargs
    )

print("bzl for test")
