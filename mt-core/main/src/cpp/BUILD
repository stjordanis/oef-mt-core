cc_binary(
    name = "app",
    visibility = [
        "//visibility:public",
    ],
    srcs = [
         "main.cpp",
    ],
    deps = [
         "//mt-core/main/src/cpp:lib",
         "@boost//:program_options",
    ],
)

cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    srcs = [
         x
         for x
         in glob(["*.cpp"])
         if x != "main.cpp"
    ],
    hdrs = glob(["*.hpp"]),
    deps = [
         "//mt-core/auth/src/cpp:lib",
         "//mt-core/comms/src/cpp:lib",
         "//mt-core/tasks-base/src/cpp:lib",
         "//mt-core/tasks/src/cpp:lib",
    ],
)
