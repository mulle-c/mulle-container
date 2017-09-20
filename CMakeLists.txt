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


set( MULLE_CONTAINER_LIBRARY ON)
include( CMakeSourcesAndHeaders.txt)
include( CMakeDependencies.txt)


set( PUBLIC_HEADERS
${HEADERS}
CMakeDependencies.txt
)


add_library( mulle_container STATIC
${SOURCES}
)


# PRIVATE is a guess
target_compile_definitions( mulle_container
PRIVATE ${MULLE_CONTAINER_DEFINITIONS}
)


set( STANDALONE_BASE_NAME "mulle_container")
set( STANDALONE_NAME "mulle_container_standalone")
set( STANDALONE_DEFINITIONS ${MULLE_CONTAINER_DEFINITIONS})
set( STANDALONE_ALL_LOAD_LIBRARIES
$<TARGET_FILE:mulle_container>
${C_DEPENDENCY_LIBRARIES}
)

include( Standalone)


INSTALL( TARGETS mulle_container mulle_container_standalone DESTINATION "lib")
INSTALL( FILES ${PUBLIC_HEADERS} DESTINATION "include/mulle_container")

include( MulleTest)
