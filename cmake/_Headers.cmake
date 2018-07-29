# cmake/_Headers.cmake is generated by `mulle-sde`. Edits will be lost.
#
if( MULLE_TRACE_INCLUDE)
   MESSAGE( STATUS "# Include \"${CMAKE_CURRENT_LIST_FILE}\"" )
endif()

set( INCLUDE_DIRS
src
src/array
src/farmhash
src/hash
src/map
src/queue
src/set
) 

set( PRIVATE_HEADERS
src/farmhash/farmhash.h
src/farmhash/largeint.h
src/include-private.h
src/map/_mulle-bigmap-private.h
)

set( PUBLIC_HEADERS
src/array/_mulle-array.h
src/array/mulle-array.h
src/array/mulle-pointerarray.h
src/array/mulle-pointerpairarray.h
src/hash/mulle-hash.h
src/hash/mulle-prime.h
src/include.h
src/map/_mulle-bigmap.h
src/map/mulle-bigmap.h
src/map/_mulle-map.h
src/map/mulle-map.h
src/map/mulle-pointerpair.h
src/mulle-container-callback-global.h
src/mulle-container-callback.h
src/mulle-container-compiler-attributes.h
src/mulle-container.h
src/mulle-container-operation.h
src/queue/_mulle-queue.h
src/set/_mulle-bigset.h
src/set/mulle-bigset.h
src/set/mulle-pointerset.h
src/set/_mulle-set.h
src/set/mulle-set.h
) 

