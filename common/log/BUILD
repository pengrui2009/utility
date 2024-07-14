load("//tools:utilities/cmake_config.bzl", "cmake_config")
load("//:modules/adaptive_autosar/ap_opts.bzl", "AP_COMMON_COPTS", "AP_COMMON_LOPTS")

cc_library(
    name = "ara-log",
    srcs = glob([
        "src/*.cpp",
        "include/private/ara/log/**/*.h",
        "include/private/nlohmann/**/*.hpp",
    ]),
    hdrs = glob([
        "include/public/ara/log/**/*.h",
    ]),
    copts = AP_COMMON_COPTS + [
        "-Werror",
        "-Wall",
    ],
    includes = [
        "include/private",
    ],
    linkopts = AP_COMMON_LOPTS,
    strip_include_prefix = "include/public/",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//modules/adaptive_autosar/Third-parties/dlt-daemon:dlt-daemon-vendor",
        "//modules/adaptive_autosar/ara-api/core:ara-core",
        "@nlohmannjson",
    ],
)

cmake_config(
    name = "ara-log-cmake",
    aliases = [
        "ara::ara-log",
        "ara-log",
    ],
    extra_cmake_package_deps = [
        "automotive-dlt",
    ],
    extra_cmake_target_deps = [
        "Genivi::dlt",
    ],
    package = "ara-log",
    target = ":ara-log",
    visibility = ["//visibility:public"],
    deps = ["//modules/adaptive_autosar/ara-api/core:ara-core-cmake"],
)
