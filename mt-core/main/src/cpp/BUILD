cc_binary(
    name = "app",
    visibility = [
        "//visibility:public",
    ],
    srcs = [
         "main.cpp",
    ],
    deps = [
         "//mt-core/tasks/src/cpp:lib",
         "//mt-core/auth/src/cpp:lib",
         "//mt-core/comms/src/cpp:lib",
         "//mt-core/main/src/cpp:lib",
    ],
)

cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    srcs = [
         "MtCore.cpp",
    ],
    hdrs = [
         "main.hpp",
         "MtCore.hpp",
    ],
    deps = [
    ],
)
