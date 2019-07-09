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
         #"//mt-core/oef-functions/src/cpp:lib",
         #"//fetch_teams/ledger/crypto:lib",
         "@openssl//:ssl",
         "@openssl//:crypto",
   ],
)
