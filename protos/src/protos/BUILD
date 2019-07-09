load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")

proto_library(
    name = "oef_core_pb",
    srcs = glob(["*.proto"]),
    visibility = [
            "//visibility:public",
    ],
)

py_proto_library(
    name = "py_oef_core_pb",
    srcs = glob(["*.proto"]),
    protoc = "@com_google_protobuf//:protoc",
    default_runtime = "@com_google_protobuf//:protobuf_python",
    visibility = ["//visibility:public"],
    deps = [
    ]
)

cc_proto_library(
    name = "cc_oef_core_pb",
    visibility = [
        "//visibility:public",
    ],
    deps = [":oef_core_pb"],
)
