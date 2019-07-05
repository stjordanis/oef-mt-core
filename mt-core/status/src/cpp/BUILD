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
   ],
)
