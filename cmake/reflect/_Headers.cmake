# This file will be regenerated by `mulle-match-to-cmake` via
# `mulle-sde reflect` and any edits will be lost.
#
# This file will be included by cmake/share/Headers.cmake
#
if( MULLE_TRACE_INCLUDE)
   MESSAGE( STATUS "# Include \"${CMAKE_CURRENT_LIST_FILE}\"" )
endif()

#
# contents are derived from the file locations

set( INCLUDE_DIRS
src
src/array
src/array/pointer
src/array/struct
src/array/uniquepointer
src/assoc
src/assoc/pointerpair
src/generic
src/map
src/map/pointer
src/queue
src/queue/struct
src/rangeset
src/reflect
src/set
src/set/pointer
)

#
# contents selected with patternfile ??-header--private-generated-headers
#
set( PRIVATE_GENERATED_HEADERS
src/reflect/_mulle-container-include-private.h
)

#
# contents selected with patternfile ??-header--private-generic-headers
#
set( PRIVATE_GENERIC_HEADERS
src/generic/include-private.h
)

#
# contents selected with patternfile ??-header--public-generated-headers
#
set( PUBLIC_GENERATED_HEADERS
src/reflect/_mulle-container-include.h
src/reflect/_mulle-container-provide.h
src/reflect/_mulle-container-versioncheck.h
)

#
# contents selected with patternfile ??-header--public-generic-headers
#
set( PUBLIC_GENERIC_HEADERS
src/generic/include.h
)

#
# contents selected with patternfile ??-header--public-headers
#
set( PUBLIC_HEADERS
src/array/mulle--array.h
src/array/mulle-array.h
src/array/mulle-flexarray.h
src/array/pointer/mulle--pointerarray.h
src/array/pointer/mulle-pointerarray.h
src/array/struct/mulle--structarray.h
src/array/struct/mulle-structarray.h
src/array/uniquepointer/mulle--uniquepointerarray.h
src/array/uniquepointer/mulle-uniquepointerarray.h
src/assoc/mulle--assoc.h
src/assoc/mulle-assoc.h
src/assoc/pointerpair/mulle--pointerpairarray.h
src/assoc/pointerpair/mulle-pointerpairarray.h
src/assoc/pointerpair/mulle-pointerpair.h
src/map/mulle--map.h
src/map/mulle-map.h
src/map/pointer/mulle--pointermap-generic.h
src/map/pointer/mulle--pointermap.h
src/map/pointer/mulle--pointermap-struct.h
src/mulle-container-callback-global.h
src/mulle-container-callback.h
src/mulle-container-compiler-attributes.h
src/mulle-container.h
src/mulle-container-math.h
src/mulle-container-operation.h
src/mulle-qsort.h
src/queue/mulle--pointerqueue.h
src/queue/mulle-pointerqueue.h
src/queue/struct/mulle--structqueue.h
src/queue/struct/mulle-structqueue.h
src/rangeset/mulle--rangeset.h
src/set/mulle--set.h
src/set/mulle-set.h
src/set/pointer/mulle--pointerset-generic.h
src/set/pointer/mulle--pointerset.h
src/set/pointer/mulle--pointerset-struct.h
)

