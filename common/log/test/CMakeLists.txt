find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

link_directories(${PROJECT_SOURCE_DIR}/lib)

enable_testing()

set(TEST_LIBRARIES gtest ara-log pthread)
set(TEST_TARGTES
    initial_test
    utility_test
    common_test
    func_test
    log_test)

foreach(target IN LISTS TEST_TARGTES)
    set(target_SRCS ${target})
    add_executable(${target} ${target_SRCS})
    target_link_libraries(${target} ${TEST_LIBRARIES})
    add_test(NAME ${target}
        COMMAND ${target})
endforeach()
