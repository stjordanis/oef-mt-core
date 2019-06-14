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
         "//cpp-utils/src/cpp/lib:lib",
         "@com_google_protobuf//:protobuf",
    ],
)
