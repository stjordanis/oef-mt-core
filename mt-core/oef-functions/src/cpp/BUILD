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
         "//mt-core/comms/src/cpp:lib",
         "//mt-core/tasks/src/cpp:lib",
         "//mt-core/conversations/src/cpp:lib",
         "//mt-core/tasks-base/src/cpp:lib",
         "//protos/src/protos:cc_oef_core_pb",
         "//fetch_teams/ledger:lib",
         "//mt-core/agents/src/cpp:lib",
         "//fetch_teams/ledger/experimental/crypto:lib",
         "@openssl//:ssl",
         "@openssl//:crypto",
   ],
)
