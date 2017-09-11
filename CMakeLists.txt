cmake_minimum_required( VERSION 3.0)

project( mulle-container C)

#
# which mulle-configuration to use ?
#
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/mulle-configuration/cmake)
message(STATUS "CMAKE_MODULE_PATH is ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  include( Debug)
else()
  include( Release)
endif()

include( CMakeSourcesAndHeaders.txt)

find_library( MULLE_ALLOCATOR_LIBRARY      NAMES mulle_allocator)
message( STATUS "MULLE_ALLOCATOR_LIBRARY is ${MULLE_ALLOCATOR_LIBRARY}")
find_library( MULLE_TEST_ALLOCATOR_LIBRARY NAMES mulle_test_allocator)
message( STATUS "MULLE_TEST_ALLOCATOR_LIBRARY is ${MULLE_TEST_ALLOCATOR_LIBRARY}")
find_library( MULLE_THREAD_LIBRARY         NAMES mulle_thread)
message( STATUS "MULLE_THREAD_LIBRARY is ${MULLE_THREAD_LIBRARY}")


set( DEPENDENCY_LIBRARIES
${MULLE_ALLOCATOR_LIBRARY}
${MULLE_TEST_ALLOCATOR_LIBRARY}
${MULLE_THREAD_LIBRARY}
)


add_library( mulle_container STATIC
${SOURCES}
)


set( TARGET_ALL_LOAD_LIBRARIES
$<TARGET_FILE:mulle_container>
${DEPENDENCY_LIBRARIES}
)

set( STANDALONE_BASE_NAME "mulle_container")
set( STANDALONE_NAME "mulle_container_standalone")

include( Standalone)

INSTALL( TARGETS mulle_container mulle_container_standalone DESTINATION "lib")
INSTALL( FILES ${HEADERS} DESTINATION "include/mulle_container")
