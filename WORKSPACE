load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "com_google_protobuf",
    sha256 = "1e622ce4b84b88b6d2cdf1db38d1a634fe2392d74f0b7b74ff98f3a51838ee53",
    strip_prefix = "protobuf-3.8.0",
    urls = ["https://github.com/protocolbuffers/protobuf/archive/v3.8.0.zip"],
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

http_archive(
    name = "six_archive",
    build_file = "@//:six.BUILD",
    sha256 = "d16a0141ec1a18405cd4ce8b4613101da75da0e9a7aec5bdd4fa804d0e0eba73",
    urls = ["https://files.pythonhosted.org/packages/dd/bf/4138e7bfb757de47d1f4b6994648ec67a51efe58fa907c1e11e350cddfca/six-1.12.0.tar.gz"],
)

bind(
  name = "six",
  actual = "@six_archive//:six",
)

http_archive(
    name = "bazel_skylib",
    sha256 = "bbccf674aa441c266df9894182d80de104cabd19be98be002f6d478aaa31574d",
    strip_prefix = "bazel-skylib-2169ae1c374aab4a09aa90e65efe1a3aad4e279b",
    urls = ["https://github.com/bazelbuild/bazel-skylib/archive/2169ae1c374aab4a09aa90e65efe1a3aad4e279b.tar.gz"],
)

#http_archive(
#    name = "pypi_six",
#    url = "https://files.pythonhosted.org/packages/dd/bf/4138e7bfb757de47d1f4b6994648ec67a51efe58fa907c1e11e350cddfca/six-1.12.0.tar.gz",
#    build_file_content = """
#py_library(
#    name = "six",
#    srcs = ["six.py"],
#    visibility = ["//visibility:public"],
#)
#    """,
#    strip_prefix = "six-1.12.0",
#)

http_archive(
    name = "protobuf_python",
    url = "https://files.pythonhosted.org/packages/65/95/8fe278158433a9bc34723f9ecbdee3097fb6baefaca932ec0331a9f80244/protobuf-3.8.0.tar.gz",
    build_file_content = """
py_library(
    name = "protobuf_python",
    srcs = glob(["google/protobuf/**/*.py"]),
    visibility = ["//visibility:public"],
    imports = [
        "@pypi_six//:six",
    ],
)
    """,
    strip_prefix = "protobuf-3.8.0",
)

new_git_repository(
    name = "googletest",
    build_file_content = """
cc_library(
    name = "gtest",
    srcs = [
         "googletest/src/gtest-all.cc",
         "googlemock/src/gmock-all.cc",
    ],
    hdrs = glob([
         "**/*.h",
         "googletest/src/*.cc",
         "googlemock/src/*.cc",
    ]),
    includes = [
        "googlemock",
        "googletest",
        "googletest/include",
        "googlemock/include",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gtest_main",
    srcs = [
         "googlemock/src/gmock_main.cc"
    ],
    linkopts = ["-pthread"
    ],
    visibility = [
         "//visibility:public"
    ],
    deps = [
         ":gtest"
    ],
)

""",
    remote = "https://github.com/google/googletest",
    tag = "release-1.8.0",
)


## proto_library, cc_proto_library, and java_proto_library rules implicitly
## depend on @com_google_protobuf for protoc and proto runtimes.
## This statement defines the @com_google_protobuf repo.
#http_archive(
#    name = "com_google_protobuf",
#    sha256 = "7b28271a61a3da0a37f6fda399b0c4c86464e5b3",
#    strip_prefix = "protobuf-3.6.1.1",
#    urls = ["https://github.com/google/protobuf/archive/v3.6.1.1.zip"],
#    visibility = ["//visibility:public"]
#)


# This is the original of the python getter replaced by the new repo rule.
#
#new_local_repository(
#    name = "python_headers",
#    path = find_python(), #"/opt/local/Library/Frameworks/Python.framework/Versions/3.6/include/python3.6m",
#    build_file_content = """
#package(
#    default_visibility = [
#        "//visibility:public",
#    ],
#)
#cc_library(
#    name = "headers",
#    srcs = glob(["**/*.h"]),
#)
#""",
#)

#load("//tools:find_python.bzl", "new_system_python_headers_repository")
#new_system_python_headers_repository(
#        name="python_headers",
#)

#load("//tools:git_archive.bzl", "project_tar_gz_repository")
#project_tar_gz_repository(
#    name="git_project",
#    path=__workspace_dir__
#)

git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "417642961150e987bc1ac78c7814c617566ffdaa",
    remote = "https://github.com/nelhage/rules_boost",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()


new_local_repository(
    name = "spdlog",
    path = "3rd",
    build_file_content = """
cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["spdlog/**/*.cpp"]),
    hdrs = glob(["spdlog/**/*.hpp", "**/*.h"]),
    copts = ["-Iexternal/"],
    deps = [
    ],
)
"""
)

new_local_repository(
    name = "mapbox",
    path = "3rd",
    build_file_content = """
cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    hdrs = glob(["mapbox/*.hpp"]),
    copts = ["-Iexternal/"],
    deps = [
    ],
)
"""
)
