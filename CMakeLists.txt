cmake_minimum_required (VERSION 3.0)

project (mulle-container)


# using include_directories is a little bit shitty
include_directories( src
src/hash
)

include_directories( SYSTEM
cityhash
cityhash/src
dependencies/include
)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -DDEBUG -DMULLE_ABA_TRACE_LIST -DMULLE_ABA_TRACE_SWAP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g -DNDEBUG")

set( HEADERS
src/mulle_container.h
src/mulle_container_callback.h
src/mulle_container_operation.h
src/mulle_container_compiler_attributes.h
src/array/mulle_array.h
src/array/_mulle_array.h
src/buffer/mulle_buffer.h
src/buffer/_mulle_buffer.h
src/hash/mulle_hash.h
src/hash/mulle_prime.h
src/map/mulle_map.h
src/map/_mulle_map.h
src/map/_mulle_indexedkeyvaluebucket.h
src/map/_mulle_keyvaluepair.h
src/queue/_mulle_queue.h
src/set/mulle_set.h
src/set/_mulle_set.h
src/set/_mulle_indexedbucket.h
src/set/_mulle_pointerset.h
)

add_library( mulle_container
src/mulle_container_callback.c
src/array/mulle_array.c
src/hash/mulle_hash.c
src/hash/mulle_prime.c
cityhash/src/city.c
src/buffer/_mulle_buffer.c
src/buffer/mulle_buffer.c
src/map/_mulle_map.c
src/map/mulle_map.c
src/map/_mulle_indexedkeyvaluebucket.c
src/queue/_mulle_queue.c
src/set/_mulle_set.c
src/set/mulle_set.c
src/set/_mulle_indexedbucket.c
src/set/_mulle_pointerset.c
)

INSTALL( TARGETS mulle_container DESTINATION "lib")
INSTALL( FILES ${HEADERS} DESTINATION "include/mulle_container")


