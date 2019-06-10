load("//:variables.bzl", "DEFINES")

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
    defines = DEFINES,
    deps = [
         "//monitoring/src/cpp/lib:lib",
         "//mt-core/auth/src/cpp:lib",
         "//mt-core/threading/src/cpp:lib",
         "//mt-core/comms/src/cpp:lib",
         "//mt-core/tasks-base/src/cpp:lib",
         "//mt-core/tasks/src/cpp:lib",
         "//mt-core/agents/src/cpp:lib",
         "//mt-core/conversations/src/cpp:lib",
         "//mt-core/oef-functions/src/cpp:lib",
	 "//cpp-utils/src/cpp/lib:lib",
	 "//mt-core/karma/src/cpp:lib",
    ],
)
