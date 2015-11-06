cmake_minimum_required (VERSION 3.0)

project (mulle-container)

#
# dont forget to wipe all cmake produced files before
# changing this
#

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -DDEBUG -DMULLE_ABA_TRACE_LIST -DMULLE_ABA_TRACE_SWAP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g -DNDEBUG")

set(HEADERS
src/mulle_container.h
src/mulle_callback_type.h
)

add_library(mulle_container
src/mulle_callback_type.c
)

ImulleTALL(TARGETS mulle_container DESTINATION "lib")
ImulleTALL(FILES ${HEADERS} DESTINATION "include/mulle_container")

