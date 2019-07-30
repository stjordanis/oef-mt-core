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
         "//basic_comms/src/cpp:lib",
         "//mt-core/karma/src/cpp:lib",
         "//mt-core/threading/src/cpp:lib",
         "//mt-core/tasks-base/src/cpp:lib",
         "//mt-core/secure/experimental/cpp:lib",
         "@com_google_protobuf//:protobuf",
         "//fetch_teams/ledger:lib",
         "//protos/src/protos:cc_oef_core_pb",
    ],
)
