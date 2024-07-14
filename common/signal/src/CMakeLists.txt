# add source files
aux_source_directory(. TIMER_SRCS)

add_library(${LIBRARY_NAME} SHARED ${TIMER_SRCS})
target_link_libraries(${LIBRARY_NAME} ara-log)