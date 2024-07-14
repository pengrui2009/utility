cmake_minimum_required(VERSION 3.4)

project(ipc_test)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# message(${GTEST_INCLUDE_DIRS})
# include_directories("../include/public")
link_directories(${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_BUILD_TYPE Debug)
set(TEST_LIBRARIES gtest pthread ara-common)
set(TEST_TARGTES test_ipc_message test_connect test_reconnect test_communication)

foreach(target IN LISTS TEST_TARGTES)
    set(target_SRCS ${target})
    add_executable(${target} ${target_SRCS})
    target_link_libraries(${target} ${TEST_LIBRARIES})
    add_test(NAME ${target}
        COMMAND ${target})
endforeach()