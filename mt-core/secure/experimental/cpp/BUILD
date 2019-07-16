load("//:variables.bzl", "DEFINES")

cc_library(
    name = "lib",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["EndpointSSL.cpp", "public_key_utils.cpp"]),
    hdrs = glob(["EndpointSSL.hpp", "public_key_utils.hpp"]),
    data = [":dh2048.pem", ":core.pem", ":core_pub.pem", ":sslclient_pub_list"],
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
    srcs = glob(["sslclient.cpp"]),
    data = [":sslclient.pem", ":core_pub.pem"],
    deps = [
         "//protos/src/protos:cc_oef_core_pb",
         "@boost//:asio",
         "@openssl//:ssl",
         "@openssl//:crypto",
   ],
)
