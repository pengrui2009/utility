cmake_minimum_required(VERSION 3.4)

project(signal_test)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

link_directories(${PROJECT_BINARY_DIR}/../src/)

set(CMAKE_BUILD_TYPE Debug)

set(TEST_LIBRARIES gtest pthread ara-common ara-log)

set(TEST_TARGTES signal_test)

foreach(target IN LISTS TEST_TARGTES)
    set(target_SRCS ${target})
    add_executable(${target} ${target_SRCS})
    target_link_libraries(${target} ${TEST_LIBRARIES})
    add_test(NAME ${target}
        COMMAND ${target})
endforeach()