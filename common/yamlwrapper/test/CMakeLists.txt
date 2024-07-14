cmake_minimum_required(VERSION 3.0.0)

project(yamlwrapper-test)

# yaml-cpp install directory
set(CMAKE_PREFIX_PATH /usr/local/)
# yaml-cpp headers directory
include_directories(/usr/local/include)

find_package(ara-yamlwrapper REQUIRED)

add_executable(${PROJECT_NAME} test.cpp)
add_executable(demo demo.cpp)

target_link_libraries(${PROJECT_NAME}  ara-yamlwrapper)
target_link_libraries(demo ara-yamlwrapper)


