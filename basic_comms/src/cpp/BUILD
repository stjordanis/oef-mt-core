load("//:variables.bzl", "DEFINES")

cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.hpp"]),
    defines = DEFINES,
    deps = [
        "@boost//:asio",
        "//cpp-utils/src/cpp/lib:lib",
        "//fetch_teams/ledger:lib",
        "//monitoring/src/cpp/lib:lib",
        "@boost//:beast",
        "//threading/src/cpp/lib:lib"
    ],
)
