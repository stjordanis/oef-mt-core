cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["logger.cpp"]),
    hdrs = glob(["logger.hpp"]),
    deps = [
        "@boost//:asio",
        "@spdlog//:lib",
        "//protos/src/protos:cc_oef_core_pb",
    ],
)
