def _var_providing_rule_impl(ctx):
    build_path = ctx.build_file_path
    loc, _ = build_path.rsplit('/', 2)

    return [platform_common.TemplateVariableInfo({"BUILD_PATH":loc}),]

var_providing_rule = rule(
    implementation = _var_providing_rule_impl,
    attrs = { "var_value": attr.string() }
)
