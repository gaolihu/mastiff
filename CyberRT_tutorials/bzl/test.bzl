def _tst_binary_impl(ctx):
    #pass
    print("ananlyzing", ctx.label)
    print("ananlyzing", ctx.label.name)

    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.write(
        output = out,
        content = "test out:\n\tHello {}!\n".format(ctx.attr.usernam) + "\t{}\n".format(ctx.attr.say),
    )
    return [DefaultInfo(files = depset([out]))]

tst_binary = rule(
    implementation = _tst_binary_impl,
    attrs = {
        "usernam": attr.string(),
        "say": attr.string(),
    }
)

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

hello_world = rule(
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

print("bzl for test")
