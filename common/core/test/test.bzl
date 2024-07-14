def ap_core_test(name):
    native.cc_test(
        name = name,
        size = "small",
        srcs = [name + ".cpp"],
        deps = [
            "@gtest//:gtest_main",
            "//modules/adaptive_autosar/ara-api/core:ara-core",
        ],
        target_compatible_with = [
            "@platforms//os:linux",
        ],
        copts = ["-fno-access-control"],
    )
