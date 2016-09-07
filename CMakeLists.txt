cmake_minimum_required (VERSION 3.0)

project (mulle-container)


#
# which mulle-configuration to use ?
#
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/mulle-configuration)
message(STATUS "CMAKE_MODULE_PATH is ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  include( Debug)
else()
  include( Release)
endif()


add_definitions( -DMULLE_ABA_TRACE_SWAP
                 -DMULLE_ALLOCATOR_EXTERN_GLOBAL=extern 
                 -DMULLE_CONTAINER_EXTERN_GLOBAL=extern)


# using include_directories is a little bit shitty
include_directories( src
src/array
src/buffer
src/hash
src/map
src/queue
src/set
)

include_directories( SYSTEM
cityhash
cityhash/src
dependencies/include
)

set( HEADERS
src/mulle_container.h
src/mulle_container_callback.h
src/mulle_container_callback_global.h
src/mulle_container_operation.h
src/mulle_container_compiler_attributes.h
src/array/mulle_array.h
src/array/_mulle_array.h
src/array/mulle_pointerarray.h
src/array/mulle_pointerpairarray.h
src/buffer/mulle_buffer.h
src/buffer/_mulle_buffer.h
src/hash/mulle_hash.h
src/hash/mulle_prime.h
src/map/mulle_bigmap.h
src/map/_mulle_bigmap.h
src/map/mulle_map.h
src/map/_mulle_map.h
src/map/mulle_pointerpair.h
src/queue/_mulle_queue.h
src/set/mulle_bigset.h
src/set/_mulle_bigset.h
src/set/mulle_set.h
src/set/_mulle_set.h
src/set/mulle_pointerset.h
)


set( SOURCES
src/mulle_container_callback.c
src/array/mulle_array.c
src/hash/mulle_hash.c
src/hash/mulle_prime.c
cityhash/src/city.c
src/buffer/_mulle_buffer.c
src/buffer/mulle_buffer.c
src/map/_mulle_bigmap.c
src/map/mulle_bigmap.c
src/map/mulle_map.c
src/map/_mulle_map.c
src/queue/_mulle_queue.c
src/set/_mulle_bigset.c
src/set/mulle_bigset.c
src/set/mulle_set.c
src/set/_mulle_set.c
src/set/mulle_pointerset.c
)

if( MSVC)
	set( DEF_FILE _mulle_container_standalone.def)
	set_source_files_properties( ${DEF_FILE} PROPERTIES HEADER_FILE_ONLY TRUE)
	set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
	set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEF:${DEF_FILE}")  
	# /NODEFAULTLIB:MSVCRT") 
endif()

add_library( mulle_container_standalone SHARED
src/mulle_container_standalone.c
${DEF_FILE}
)

add_library(mulle_container STATIC
${SOURCES}
)

set( DEPENDENCY_LIBRARIES
mulle_test_allocator
mulle_allocator
mulle_thread
)

add_dependencies( mulle_container_standalone mulle_container)

if( MSVC)
	add_custom_command( OUTPUT ${DEF_FILE}
                 	    COMMAND mulle-mingw-dumpdef.bat -o "${DEF_FILE}" -n mulle_container_standalone --directory "${BUILD_RELATIVE_DEPENDENCIES_DIR}/lib" --prefix "mulle" ${DEPENDENCY_LIBRARIES} $<TARGET_FILE:mulle_container>
                 	    DEPENDS "${SOURCES}" "${HEADERS}"
                   		VERBATIM)
endif()


TARGET_LINK_LIBRARIES( mulle_container_standalone
${BEGIN_ALL_LOAD}
mulle_container
${DEPENDENCY_LIBRARIES}
${END_ALL_LOAD}
)

INSTALL( TARGETS mulle_container mulle_container_standalone DESTINATION "lib")
INSTALL( FILES ${HEADERS} DESTINATION "include/mulle_container")


