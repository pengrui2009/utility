cmake_minimum_required(VERSION 3.4)

project(ap_log
	VERSION 1.0.0
	LANGUAGES CXX
)

include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# find_package(ap_vendor REQUIRED)
# find_package(ara-core REQUIRED)
# find_package(PkgConfig REQUIRED)
# pkg_search_module(DLT automotive-dlt REQUIRED)


INCLUDE_DIRECTORIES(${ARA_CORE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(include/public)
INCLUDE_DIRECTORIES(include/private)

# dependencies
if(${CMAKE_SYSTEM_NAME} MATCHES "QNX")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -D_XOPEN_SOURCE=600")
else()
	set(CMAKE_THREAD_PREFER_PTHREAD ON)

	if("${CMAKE_VERSION}" VERSION_EQUAL "3.10.0" OR "${CMAKE_VERSION}" VERSION_GREATER "3.10.0")
		# This leads to build failure when cross-compiling in CMake below 3.10.0
		set(THREADS_PREFER_PTHREAD_FLAG ON)
	endif()

	if("${CMAKE_VERSION}" VERSION_LESS "3.10.0" AND CMAKE_USE_PTHREADS_INIT)
		# Fallback to modifying CXX_FLAGS in older versions
		# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
		set(CMAKE_CXX_FLAGS "-std=c++14 -pthread")
	endif()
endif()

add_subdirectory(include/public)
add_subdirectory(src)

if(ARA_ENABLE_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()
