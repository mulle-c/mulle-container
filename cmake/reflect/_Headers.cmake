#
# cmake/reflect/_Headers.cmake is generated by `mulle-sde reflect`. Edits will be lost.
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
src/range
src/rangeset
src/reflect
src/set
)

set( PRIVATE_GENERATED_HEADERS
src/reflect/_mulle-container-include-private.h
)

set( PRIVATE_HEADERS
src/farmhash/farmhash.h
src/farmhash/largeint.h
src/include-private.h
)

set( PUBLIC_GENERATED_HEADERS
src/reflect/_mulle-container-include.h
)

set( PUBLIC_HEADERS
src/array/mulle--array.h
src/array/mulle-array.h
src/array/mulle-pointerarray.h
src/array/mulle-pointerpairarray.h
src/array/mulle-structarray.h
src/hash/mulle-hash.h
src/hash/mulle-prime.h
src/include.h
src/map/mulle--map.h
src/map/mulle-map.h
src/map/mulle--pointermap.h
src/map/mulle-pointerpair.h
src/mulle-container-callback-global.h
src/mulle-container-callback.h
src/mulle-container-compiler-attributes.h
src/mulle-container.h
src/mulle-container-math.h
src/mulle-container-operation.h
src/queue/mulle--pointerqueue.h
src/range/mulle-range.h
src/rangeset/mulle--rangeset.h
src/set/mulle-pointerset.h
src/set/mulle--set.h
src/set/mulle-set.h
)
