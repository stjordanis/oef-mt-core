load("//:variables.bzl", "DEFINES")

cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["EndpointSSL.cpp"]),
    hdrs = glob(["EndpointSSL.hpp"]),
    data = [":dh2048.pem",":server.pem"],
    defines = DEFINES,
    deps = [
         #"//fetch_teams/ledger/crypto:lib",
         "//basic_comms/src/cpp:lib",
         #"//mt-core/comms/src/cpp:lib",
         #"//mt-core/oef-functions/src/cpp:lib",
         "@openssl//:ssl",
         "@openssl//:crypto",
   ],
)


cc_binary(
    name = "sslclient",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["client.cpp"]),
    data = [":ca.pem"],
    deps = [
         "//protos/src/protos:cc_oef_core_pb",
         "@boost//:asio",
         "@openssl//:ssl",
         "@openssl//:crypto",
   ],
)
