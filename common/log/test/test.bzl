def ap_log_test(name):
    native.cc_test(
        name = name,
        size = "small",
        srcs = [name + ".cpp"],
        deps = [
            "@gtest//:gtest_main",
            "//modules/adaptive_autosar/ara-api/log:ara-log",
        ],
        target_compatible_with = [
            "@platforms//os:linux",
        ],
    )
