function(build_library project_name)
    project(${project_name} VERSION 1.0.0 LANGUAGES CXX)

    set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    file(GLOB all_SRCS
            "${PROJECT_SOURCE_DIR}/include/public/*.h"
            "${PROJECT_SOURCE_DIR}/include/public/*.hpp"
            "${PROJECT_SOURCE_DIR}/include/public/internal/*.h"
            "${PROJECT_SOURCE_DIR}/include/public/internal/*.hpp"
            "${PROJECT_SOURCE_DIR}/src/*.cpp"
            "${PROJECT_SOURCE_DIR}/src/*.c"
            )

    file(GLOB all_headers
            "${PROJECT_SOURCE_DIR}/include/*.h"
            "${PROJECT_SOURCE_DIR}/include/*.hpp"
            "${PROJECT_SOURCE_DIR}/include/public/internal/*.h"
            "${PROJECT_SOURCE_DIR}/include/public/internal/*.hpp"
            )

    include_directories(${PROJECT_SOURCE_DIR}/include/public/)

    add_library(${project_name} ${all_SRCS})
    set_target_properties(${project_name} PROPERTIES PUBLIC_HEADER "${all_headers}")

    #
    #    if(IS_OUTPUT_PACKAGE)
    #        include(FindPkgConfig)
    #        if(PKG_CONFIG_FOUND)
    #	    set(LIBRARIES "-l${project_name}")
    #            configure_file(${PROJECT_SOURCE_DIR}/files/pkgconfig/${project_name}.pc.in ${project_name}.pc @ONLY)
    #            install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${project_name}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
    #        endif()
    #
    #        include(CMakePackageConfigHelpers)
    #        configure_package_config_file(
    #            ${PROJECT_SOURCE_DIR}/files/cmake/${project_name}-config.cmake.in
    #            ${CMAKE_CURRENT_BINARY_DIR}/${project_name}-config.cmake
    #            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${project_name}
    #            NO_CHECK_REQUIRED_COMPONENTS_MACRO
    #        )
    #        # Create *-config-version.cmake file
    #        write_basic_package_version_file(
    #            ${CMAKE_CURRENT_BINARY_DIR}/${project_name}-config-version.cmake
    #            COMPATIBILITY ExactVersion
    #        )
    #        # Install *-config.cmake and *-version.cmake files
    #        install(
    #            FILES
    #                ${CMAKE_CURRENT_BINARY_DIR}/${project_name}-config.cmake
    #                ${CMAKE_CURRENT_BINARY_DIR}/${project_name}-config-version.cmake
    #            DESTINATION
    #                ${CMAKE_INSTALL_LIBDIR}/cmake/${project_name}
    #        )
    #        
    #        install(
    #            TARGETS ${project_name}
    #            EXPORT ${project_name}-targets
    #            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    #            PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/ara-core"
    #        )
    #        install(
    #            EXPORT ${project_name}-targets
    #            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${project_name}
    #            NAMESPACE ${project_name}::
    #        )
    #    endif()
endfunction()
function(build_test project_name)
    find_package(GTest REQUIRED)

    set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    file(GLOB all_SRCS
            "${PROJECT_SOURCE_DIR}/test/${project_name}.cpp"
            )

    include_directories(${PROJECT_SOURCE_DIR}/include/public)

    add_compile_options(-fno-access-control)
    add_executable(${project_name} "${PROJECT_SOURCE_DIR}/test/${project_name}.cpp")
    target_link_libraries(${project_name} ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    target_include_directories(${project_name} PUBLIC core)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${project_name}
        DESTINATION ${PROJECT_SOURCE_DIR}/test/)
    set(CMAKE_CLEAN_FILES "${PROJECT_SOURCE_DIR}/test/${project_name};${CMAKE_CLEAN_FILES}" CACHE STRING INSTERNAL FORCE)
endfunction()
