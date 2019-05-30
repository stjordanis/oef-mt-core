cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.hpp"]),
    deps = [
         "//threading/src/cpp/lib:lib",
         "//mt-core/tasks-base/src/cpp:lib",
         "//mt-core/tasks/src/cpp:lib",
         "//mt-core/comms/src/cpp:lib",
         "@com_google_protobuf//:protobuf",
    ],
)
