load("//tools:utilities/cmake_config.bzl", "cmake_config")
load("//:modules/adaptive_autosar/ap_opts.bzl", "AP_COMMON_COPTS", "AP_COMMON_LOPTS", "AP_QNX_WITH_BOOST_COPTS")

cc_library(
    name = "platform_error_domain",
    hdrs = glob([
        "platform_error_domain/include/public/**/*.h",
    ]),
    copts = AP_COMMON_COPTS,
    linkopts = AP_COMMON_LOPTS,
    strip_include_prefix = "platform_error_domain/include/public/",
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "ara-core",
    srcs = glob([
        "src/*.cpp",
        "src/*.c",
    ]),
    hdrs = glob([
        "include/public/ara/core/**/*.h",
        "include/public/ara/core/**/*.hpp",
    ]),
    copts = AP_COMMON_COPTS,
    linkopts = AP_COMMON_LOPTS,
    strip_include_prefix = "include/public/",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":platform_error_domain",
    ]
)

cmake_config(
    name = "ara-core-cmake",
    aliases = ["ara::ara-core", "ara-core"],
    package = "ara-core",
    target = ":ara-core",
    visibility = ["//visibility:public"],
)
