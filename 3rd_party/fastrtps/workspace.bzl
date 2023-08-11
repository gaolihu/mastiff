"""Load the fastrtps library"""

# Sanitize a dependency so that it works correctly from code that includes
# Apollo as a submodule.
def clean_dep(dep):
    return str(Label(dep))

def repo():
    native.new_local_repository(
        name = "fastcdr",
        build_file = clean_dep("//@FASTRTPS_PKG@/fastrtps:fastcdr.BUILD"),
        path = "@FASTRTPS_PKG_PATH@",
    )

    native.new_local_repository(
        name = "fastrtps",
        build_file = clean_dep("//@FASTRTPS_PKG@/fastrtps:fastrtps.BUILD"),
        path = "@FASTRTPS_PKG_PATH@",
    )
