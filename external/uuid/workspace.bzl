"""Loads the uuid library"""

# Sanitize a dependency so that it works correctly from code that includes
# Apollo as a submodule.
def clean_dep(dep):
    return str(Label(dep))

# Installed via uuid-dev
def repo():
    # uuid
    native.new_local_repository(
        name = "uuid",
        build_file = clean_dep("//@UUID_PKG@/uuid:uuid.BUILD"),
        path = "@UUID_PKG_PATH@",
    )
