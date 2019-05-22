cc_library(
    name = "liboef-old",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.hpp","api/*.hpp"]),
    deps = [
        "@boost//:asio",
        "@spdlog//:lib",
        "@mapbox//:lib",
        "//protos/src/protos:cc_oef_core_pb",
    ],
)
