load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")

proto_library(
    name = "text_server_pb",
    srcs = glob(["*.proto"]),
    visibility = [
            "//visibility:public",
    ],
)

py_proto_library(
    name = "py_text_server_pb",
    srcs = glob(["*.proto"]),
    protoc = "@com_google_protobuf//:protoc",
    default_runtime = "@com_google_protobuf//:protobuf_python",
    visibility = ["//visibility:public"],
    deps = [
    ]
)

cc_proto_library(
    name = "cc_text_server_pb",
    visibility = [
        "//visibility:public",
    ],
    deps = [":text_server_pb"],
)
