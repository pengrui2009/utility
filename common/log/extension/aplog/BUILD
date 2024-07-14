load("//tools:utilities/cc_extensions.bzl", "cc_shared_library")

cc_library(
    name = "ara-extend-aplog",
    hdrs = glob(["public/**/*.h"]),
    strip_include_prefix = "public/",
    visibility = ["//visibility:public"],
    deps = [
        "//modules/adaptive_autosar/ara-api/log:ara-log",
        "@fmt",
    ],
)
