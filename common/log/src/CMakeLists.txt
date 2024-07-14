# add source files
aux_source_directory(. LOG_SRCS)

link_directories(${DLT_LIBRARY_DIRS})

set(LIBRARY_NAME ara-log)

add_library(${LIBRARY_NAME} SHARED ${LOG_SRCS})
target_link_libraries(${LIBRARY_NAME} PRIVATE dlt)

# wirite lib version
include(CMakePackageConfigHelpers)

set(ARA_LOG_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
set(ARA_LOG_LIBRARIES_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/ara-logConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ara-logConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    PATH_VARS ARA_LOG_INCLUDE_DIR ARA_LOG_LIBRARIES_DIR
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ara-logConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

install(TARGETS ${LIBRARY_NAME}
    EXPORT ara-logTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT ara-logTargets
    FILE ara-logTargets.cmake
    NAMESPACE ara::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

# configure_file(ara-logConfig.cmake.in ara-logConfig.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ara-logConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ara-logConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

INSTALL(FILES Log_configure.json
    DESTINATION etc/ara-log)

# add_adaptive_library(${PROJECT_NAME}
# SOURCES
# ${LOG_SRCS}
# LIBS
# ${DLT_LIBRARIES}
# Threads::Threads
# NAMESPACE
# ara
# INCLUDE_DIRECTORIES
# ${PROJECT_SOURCE_DIR}/include/public
# )

# set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
