# Copyright 2022 tusen authors.

cmake_minimum_required(VERSION 3.5.1)

project(utility)

set(CMAKE_CXX_STANDARD 14)
# set(FMTLIB_PROVIDER "module" CACHE STRING "Provider of fmtlib library")
# set_property(CACHE FMTLIB_PROVIDER PROPERTY STRINGS "module" "package")

# include(cmake/fmtlib.cmake)

include_directories(
    include
    # base
    # base/framework
)

# framework_lib
# add_library(fmlib
#     "base/MasterHandle.cc"
#     "base/NodeHandle.cc"
#     "base/framework/msg.cc"
#     "base/framework/message.cc"
# )
# target_link_libraries(fmlib
#     fmt
#     pthread
# )

# example
add_executable(task_demo 
    "examples/task_demo.cpp"
)
target_link_libraries(task_demo
    pthread
)
# install(TARGETS server DESTINATION lib/${PROJECT_NAME})

add_executable(executor_demo 
    "examples/executor_demo.cpp"
)
target_link_libraries(executor_demo
    pthread
)
# install(TARGETS client DESTINATION lib/${PROJECT_NAME})

install(TARGETS task_demo executor_demo DESTINATION lib/${PROJECT_NAME})
