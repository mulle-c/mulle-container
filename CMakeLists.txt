cmake_minimum_required( VERSION 3.0)

project( mulle-container C)


### mulle-sde environment

# add cmake module paths to search path
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake/share")
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake")

include( Environment)


### Files

include( PreFiles OPTIONAL)

set( MULLE_CONTAINER_LIBRARY ON)

#
# Include all non underscored .cmake files in ./cmake
#
file( GLOB LOCAL_CMAKE_INCLUDES
   "cmake/[A-Za-z0-9]*.cmake"
)

foreach( FILE ${LOCAL_CMAKE_INCLUDES} )
   include( "${FILE}")
endforeach()


include_directories( ${INCLUDE_DIRS})

include( PostFiles OPTIONAL)

### Library

include( PreLibrary OPTIONAL)

add_library( "mulle-container"
${SOURCES}
${PUBLIC_HEADERS}
${PRIVATE_HEADERS}
${CMAKE_EDITABLE_FILES}
)

if( BUILD_SHARED_LIBS)
   target_link_libraries( mulle-container
${DEPENDENCIES_LIBRARIES}
${OS_SPECIFIC_LIBRARIES}
)
endif()

include( PostLibrary OPTIONAL)

### Install

include( PreInstall OPTIONAL)

install( TARGETS "mulle-container" DESTINATION "lib")
install( FILES ${PUBLIC_HEADERS}  DESTINATION "include/mulle-container")
install( FILES ${PRIVATE_HEADERS} DESTINATION "include/mulle-container/private")

install( FILES ${CMAKE_INCLUDES}  DESTINATION "include/mulle-container/cmake")

include( PostInstall OPTIONAL)
