cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(threadpool_test
    LANGUAGES CXX
    VERSION 1.0.0
)

include(GNUInstallDirs)
find_package(GTest)
include_directories(${GTEST_INCLUDE_DIRS})

enable_testing()

if(${CMAKE_SYSTEM_NAME} MATCHES "QNX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -D_XOPEN_SOURCE=600")
else()
    set(CMAKE_CXX_FLAGS "-std=c++14 -pthread")
endif()

set(ARA_THREADPOOL_INC
    "../include/public")
MESSAGE(STATUS "ARA THREAD POOL INC: ${ARA_THREADPOOL_INC}")
set(target threadpool_test)
set(TEST_LIBRARIES gtest)

include_directories(${ARA_THREADPOOL_INC})

add_executable(${target} threadpool_test.cc)
target_link_libraries(${target} ${TEST_LIBRARIES})
add_test(NAME ${target}
    COMMAND ${target})
