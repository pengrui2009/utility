

cmake_minimum_required(VERSION 3.5)

project(ara-core VERSION 1.0.0 LANGUAGES CXX)



include(GNUInstallDirs)



# Subdir for include files at install location.
# We'll need this for defining the *_INCLUDE_DIRS variable in our installed *-config.cmake file.

set(ARA_CORE_INCLUDE_DIR "include")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${CMAKE_SYSTEM_NAME} MATCHES "QNX")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -D_XOPEN_SOURCE=500")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pthread -g")
endif()

add_library(core INTERFACE)

target_include_directories(core INTERFACE
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/public>
	$<INSTALL_INTERFACE:${ARA_CORE_INCLUDE_DIR}>
)

add_library(ara::core ALIAS core)

include(${PROJECT_SOURCE_DIR}/build.cmake)


if(ARA_ENABLE_TESTS)
	enable_testing()
    find_package(GTest REQUIRED)
    find_library(GMOCK gmock)
    build_library(ara-core-all-headers)
    build_test(array_test)
    build_test(map_test)
    build_test(string_test)
    build_test(vector_test)
    build_test(string_view_test)
    build_test(error_code_test)
    build_test(result_test)
    build_test(optional_test)
    build_test(core_error_domain_test)
    build_test(future_error_domain_test)
    build_test(promise_test)
    build_test(function_test)
endif()

# -----------------------------

include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    configure_file(${PROJECT_SOURCE_DIR}/files/pkgconfig/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

# Create *-config.cmake file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/files/cmake/${PROJECT_NAME}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    PATH_VARS ARA_CORE_INCLUDE_DIR
    NO_CHECK_REQUIRED_COMPONENTS_MACRO # ara::core does not provide components
)
# Create *-config-version.cmake file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    COMPATIBILITY ExactVersion
)
# Install *-config.cmake and *-version.cmake files
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Install exported targets, i.e. *-targets.cmake
install(
    TARGETS core
    EXPORT ${PROJECT_NAME}-targets
)
install(
    EXPORT ${PROJECT_NAME}-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NAMESPACE ara::
)

# Install header files
install(
    DIRECTORY include/public/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)
