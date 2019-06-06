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
         "//threading/src/cpp/lib:lib",
         "//mt-core/tasks-base/src/cpp:lib",
         "//mt-core/comms/src/cpp:lib",
         "//protos/src/protos:cc_oef_core_pb",
         "@com_google_protobuf//:protobuf",
         "//fetch_teams/ledger:lib",
         "//mt-core/tasks/src/cpp:lib"
    ],
)
