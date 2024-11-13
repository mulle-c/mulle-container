cmake_minimum_required( VERSION 3.5)

project( test C)

add_subdirectory( ../../../mulle-c11        "${CMAKE_CURRENT_BINARY_DIR}/mulle-c11")
add_subdirectory( ../../../mulle-allocator  "${CMAKE_CURRENT_BINARY_DIR}/mulle-allocator")  # needed for mulle-testallocator
add_subdirectory( ../../../mulle-data       "${CMAKE_CURRENT_BINARY_DIR}/mulle-data")
add_subdirectory( ../../../mulle-container  "${CMAKE_CURRENT_BINARY_DIR}/mulle-container")

add_executable( ${PROJECT_NAME} main.c)
set_target_properties( ${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "package"
    SUFFIX ".exe"
)

target_link_libraries( ${PROJECT_NAME} PRIVATE mulle-c11)
target_link_libraries( ${PROJECT_NAME} PRIVATE mulle-allocator)
target_link_libraries( ${PROJECT_NAME} PRIVATE mulle-data)
target_link_libraries( ${PROJECT_NAME} PRIVATE mulle-container)

