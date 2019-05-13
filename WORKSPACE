load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "com_google_protobuf",
    sha256 = "9510dd2afc29e7245e9e884336f848c8a6600a14ae726adb6befdb4f786f0be2",
    strip_prefix = "protobuf-3.6.1.3",
    urls = ["https://github.com/google/protobuf/archive/v3.7.1.zip"],
)

http_archive(
    name = "six_archive",
    build_file_content = """
genrule(
  name = "copy_six",
  srcs = ["six-1.10.0/six.py"],
  outs = ["six.py"],
  cmd = "cp $< $(@)",
)

py_library(
  name = "six",
  srcs = ["six.py"],
  srcs_version = "PY2AND3",
  visibility = ["//visibility:public"],
)
""",
#    sha256 = "105f8d68616f8248e24bf0e9372ef04d3cc10104f1980f54d57b2ce73a5ad56a",
    urls = [
          "https://pypi.python.org/packages/source/s/six/six-1.10.0.tar.gz",
    ],
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
#    url = "https://pypi.org/packages/16/d8/bc6316cf98419719bd59c91742194c111b6f2e85abac88e496adefaf7afe/six-1.11.0.tar.gz",
#    build_file_content = """
#py_library(
#    name = "six",
#    srcs = ["six.py"],
#    visibility = ["//visibility:public"],
#)
#    """,
#    strip_prefix = "six-1.11.0",
#)

http_archive(
    name = "protobuf_python",
    url = "https://files.pythonhosted.org/packages/1b/90/f531329e628ff34aee79b0b9523196eb7b5b6b398f112bb0c03b24ab1973/protobuf-3.6.1.tar.gz",
    build_file_content = """
py_library(
    name = "protobuf_python",
    srcs = glob(["google/protobuf/**/*.py"]),
    visibility = ["//visibility:public"],
    imports = [
        "//six:six",
    ],
)
    """,
    strip_prefix = "protobuf-3.6.1",
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
    commit = "6d6fd834281cb8f8e758dd9ad76df86304bf1869",
    remote = "https://github.com/nelhage/rules_boost",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()
