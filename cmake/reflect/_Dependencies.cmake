# This file will be regenerated by `mulle-sourcetree-to-cmake` via
# `mulle-sde reflect` and any edits will be lost.
#
# This file will be included by cmake/share/Files.cmake
#
# Disable generation of this file with:
#
# mulle-sde environment set MULLE_SOURCETREE_TO_CMAKE_DEPENDENCIES_FILE DISABLE
#
if( MULLE_TRACE_INCLUDE)
   message( STATUS "# Include \"${CMAKE_CURRENT_LIST_FILE}\"" )
endif()

#
# Generated from sourcetree: 0A5269CA-C179-4449-A0E4-D787842FC0CD;mulle-allocator;no-all-load,no-cmake-inherit,no-import,no-singlephase;
# Disable with : `mulle-sourcetree mark mulle-allocator no-link`
# Disable for this platform: `mulle-sourcetree mark mulle-allocator no-cmake-platform-${MULLE_UNAME}`
# Disable for a sdk: `mulle-sourcetree mark mulle-allocator no-cmake-sdk-<name>`
#
if( NOT MULLE__ALLOCATOR_LIBRARY)
   find_library( MULLE__ALLOCATOR_LIBRARY NAMES
      ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-allocator${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-allocator${CMAKE_STATIC_LIBRARY_SUFFIX}
      mulle-allocator
      NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH
   )
   if( NOT MULLE__ALLOCATOR_LIBRARY AND NOT DEPENDENCY_IGNORE_SYSTEM_LIBARIES)
      find_library( MULLE__ALLOCATOR_LIBRARY NAMES
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-allocator${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-allocator${CMAKE_STATIC_LIBRARY_SUFFIX}
         mulle-allocator
      )
   endif()
   message( STATUS "MULLE__ALLOCATOR_LIBRARY is ${MULLE__ALLOCATOR_LIBRARY}")
   #
   # The order looks ascending, but due to the way this file is read
   # it ends up being descending, which is what we need.
   #
   if( MULLE__ALLOCATOR_LIBRARY)
      #
      # Add MULLE__ALLOCATOR_LIBRARY to DEPENDENCY_LIBRARIES list.
      # Disable with: `mulle-sourcetree mark mulle-allocator no-cmake-add`
      #
      list( APPEND DEPENDENCY_LIBRARIES ${MULLE__ALLOCATOR_LIBRARY})
      # intentionally left blank
   else()
      # Disable with: `mulle-sourcetree mark mulle-allocator no-require-link`
      message( FATAL_ERROR "MULLE__ALLOCATOR_LIBRARY was not found")
   endif()
endif()


#
# Generated from sourcetree: 7CC3FBD7-0833-4FE8-AB5C-A4F092FF345C;mulle-data;no-all-load,no-cmake-inherit,no-import,no-singlephase;
# Disable with : `mulle-sourcetree mark mulle-data no-link`
# Disable for this platform: `mulle-sourcetree mark mulle-data no-cmake-platform-${MULLE_UNAME}`
# Disable for a sdk: `mulle-sourcetree mark mulle-data no-cmake-sdk-<name>`
#
if( NOT MULLE__DATA_LIBRARY)
   find_library( MULLE__DATA_LIBRARY NAMES
      ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-data${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-data${CMAKE_STATIC_LIBRARY_SUFFIX}
      mulle-data
      NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH
   )
   if( NOT MULLE__DATA_LIBRARY AND NOT DEPENDENCY_IGNORE_SYSTEM_LIBARIES)
      find_library( MULLE__DATA_LIBRARY NAMES
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-data${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
         ${CMAKE_STATIC_LIBRARY_PREFIX}mulle-data${CMAKE_STATIC_LIBRARY_SUFFIX}
         mulle-data
      )
   endif()
   message( STATUS "MULLE__DATA_LIBRARY is ${MULLE__DATA_LIBRARY}")
   #
   # The order looks ascending, but due to the way this file is read
   # it ends up being descending, which is what we need.
   #
   if( MULLE__DATA_LIBRARY)
      #
      # Add MULLE__DATA_LIBRARY to DEPENDENCY_LIBRARIES list.
      # Disable with: `mulle-sourcetree mark mulle-data no-cmake-add`
      #
      list( APPEND DEPENDENCY_LIBRARIES ${MULLE__DATA_LIBRARY})
      # intentionally left blank
   else()
      # Disable with: `mulle-sourcetree mark mulle-data no-require-link`
      message( FATAL_ERROR "MULLE__DATA_LIBRARY was not found")
   endif()
endif()
