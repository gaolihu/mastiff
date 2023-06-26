load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")
load(":test.bzl", "tst_binary", "hello_world")

print("BUILD for test")

tst_binary(
    name = "ttt",
    usernam = "Lihu",
    say = "Are U OK?"
)

#-----------

genrule(
    name = "inject-version",
    outs = ["VERSION"],
    local = 1,
    cmd = "git rev-parse HEAD",
)

hello_world(
    name = "hi",
    user = "Lihu",
    version = ":inject-version",
    template = ":hlo.cc.tpl",
)

cc_binary(
    name = "hello",
    srcs = [":hi"],
)
