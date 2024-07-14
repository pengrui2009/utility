# add source files
aux_source_directory(. IPC_SRCS)

# link_libraries(${DLT_LIBRARIES})
# if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "QNX")
# link_libraries(Threads::Threads)
# endif()
add_library(${LIBRARY_NAME} SHARED ${IPC_SRCS})
target_link_libraries(${LIBRARY_NAME} PRIVATE ara::ara-log)
