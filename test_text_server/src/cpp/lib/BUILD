cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.hpp"]),
    deps = [
        "@boost//:asio",
        "@com_google_protobuf//:protobuf",
        "//basic_comms/src/cpp:lib",
        "//test_text_server/src/protos:cc_text_server_pb",
        "//threading/src/cpp/lib:lib",
    ],
)
