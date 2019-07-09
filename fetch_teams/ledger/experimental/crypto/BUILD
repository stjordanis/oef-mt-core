cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.hpp"]),
    deps = [
         "//fetch_teams/ledger/experimental/core:lib",
         "//fetch_teams/ledger/experimental/meta:lib",
         "//fetch_teams/ledger/experimental/vectorise:lib",
         "@openssl//:ssl",
         "@openssl//:crypto",
   ],
)
