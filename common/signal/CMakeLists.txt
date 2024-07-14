cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(ara-signal VERSION 1.0 LANGUAGES CXX)

if(TSPKG_BUILD)
    set(Boost_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/Boost-1.71.0)
    link_directories(BEFORE ${CMAKE_INSTALL_PREFIX}/lib)
    INCLUDE_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/include/)

    if(${CMAKE_SYSTEM_NAME} MATCHES "QNX")
        # use std::string_view for llvm
        add_definitions(-DBOOST_ASIO_HAS_STD_STRING_VIEW=1)
        add_definitions(-DBOOST_ASIO_HAS_STD_EXPERIMENTAL_STRING_VIEW=0)
    endif()
endif()

find_package(Boost REQUIRED)
find_package(ara-log REQUIRED)

INCLUDE_DIRECTORIES(include/public)
INCLUDE_DIRECTORIES(include/private)

set(LIBRARY_NAME ara-signal)

# add_subdirectory(include/public)
add_subdirectory(src)

if(ARA_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()