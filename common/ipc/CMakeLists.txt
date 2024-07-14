cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(ara-ipc
    LANGUAGES CXX
    VERSION 1.0.0
)
include(GNUInstallDirs)

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

find_package(ara-core REQUIRED)
find_package(ara-log REQUIRED)
find_package(Boost REQUIRED)

INCLUDE_DIRECTORIES(${ARA_CORE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(include/public)
INCLUDE_DIRECTORIES(include/private)

if(${CMAKE_SYSTEM_NAME} MATCHES "QNX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_XOPEN_SOURCE=600")

    # set(CMAKE_THREAD_PREFER_PTHREAD ON)

    # if("${CMAKE_VERSION}" VERSION_EQUAL "3.10.0" OR "${CMAKE_VERSION}" VERSION_GREATER "3.10.0")
    # # This leads to build failure when cross-compiling in CMake below 3.10.0
    # set(THREADS_PREFER_PTHREAD_FLAG ON)
    # endif()

    # find_package(Threads REQUIRED)
    # if("${CMAKE_VERSION}" VERSION_LESS "3.10.0" AND CMAKE_USE_PTHREADS_INIT)
    # # Fallback to modifying CXX_FLAGS in older versions
    # # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    # # set(CMAKE_CXX_FLAGS "-std=c++14 -pthread")
    # endif()
endif()

set(LIBRARY_NAME ara-ipc)

add_subdirectory(include/public)
add_subdirectory(src)

if(ARA_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# wirite lib version
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

install(TARGETS ${LIBRARY_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ara::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)