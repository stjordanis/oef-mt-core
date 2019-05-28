cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.hpp"]),
    deps = [
         "//basic_comms/src/cpp:lib",
         "//mt-core/threading/src/cpp:lib",
         "//mt-core/tasks-base/src/cpp:lib",
         "@com_google_protobuf//:protobuf",
    ],
)
