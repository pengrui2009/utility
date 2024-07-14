filegroup(
    name = "config",
    srcs = glob([
        "yaml_config_root/**/*",
        "yaml_config_root/*",
    ]),
)

cc_test(
    name = "get-yaml-test",
    srcs = [
        "get_yaml_test.cpp",
    ],
    deps = [
        "//modules/adaptive_autosar/ara-api/common:ara-yamlwrapper",
        "@gtest//:gtest_main",
    ],
    data = [
        ":config",
    ],
)
cc_test(
    name = "get-dir-test",
    srcs = [
        "get_dir_test.cpp",
    ],
    deps = [
        "//modules/adaptive_autosar/ara-api/common:ara-yamlwrapper",
        "@gtest//:gtest_main",
    ],
    data = [
        ":config",
    ],
)

cc_binary(
    name = "test",
    srcs = [
        "test.cpp",
    ],
    deps = [
        "//modules/adaptive_autosar/ara-api/common:ara-yamlwrapper",
    ],
    data = [
        ":config",
    ],
)

cc_binary(
    name = "demo",
    srcs = [
        "demo.cpp",
    ],
    deps = [
        "//modules/adaptive_autosar/ara-api/common:ara-yamlwrapper",
    ],
)