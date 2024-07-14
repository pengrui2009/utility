cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(ara-timer
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

find_package(Boost REQUIRED)

INCLUDE_DIRECTORIES(${ARA_CORE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(include/public)
INCLUDE_DIRECTORIES(include/private)

if(${CMAKE_SYSTEM_NAME} MATCHES "QNX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_XOPEN_SOURCE=600")
endif()

set(LIBRARY_NAME ara-timer)

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