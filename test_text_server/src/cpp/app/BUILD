cc_binary(
    name = "app",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["main.cpp"]),
    deps = [
        "//basic_comms/src/cpp:lib",
        "//threading/src/cpp/lib:lib",
        "//test_text_server/src/cpp/lib:lib",
    ],
    data = [
    ]
    #linkshared = True,
    #linkstatic = True,
)

cc_binary(
    name = "app2",
    visibility = [
        "//visibility:public",
    ],
    srcs = glob(["main-protos.cpp"]),
    deps = [
        "//basic_comms/src/cpp:lib",
        "//threading/src/cpp/lib:lib",
        "//test_text_server/src/cpp/lib:lib",
    ],
    data = [
    ]
    #linkshared = True,
    #linkstatic = True,
)
