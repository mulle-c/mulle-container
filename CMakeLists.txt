cmake_minimum_required( VERSION 3.13...99.99)

project( mulle-container VERSION 10.1.4 LANGUAGES C)

#
# The library advertises ANSI/ISO C (see the "std" and "apis" badges in the
# README). Compile it as C11, so that this is exercised rather than merely
# claimed, and define no feature test macro at all: strict C mode then offers
# only the standard declarations, so a POSIX or BSD call is an error instead of
# passing silently. That is what "apis: ansi" claims, so the build checks it too.
#
# A standard given on the command line wins, so other dialects remain testable
# with e.g. -DCMAKE_C_STANDARD=23 or CFLAGS=-std=c23.
#
if( NOT CMAKE_C_STANDARD
    AND NOT CMAKE_C_FLAGS MATCHES "(^| )-std="
    AND NOT "$ENV{CFLAGS}" MATCHES "(^| )-std=")
   set( CMAKE_C_STANDARD 11)
   set( CMAKE_C_EXTENSIONS OFF)
endif()




# add cmake module paths to search path
list( INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/share")
list( INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/reflect")
list( INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include( Environment)

include( Files)

### Library

if( HEADERS_PHASE)
   install( FILES ${INSTALL_PUBLIC_HEADERS}  DESTINATION "include/mulle-container")
   install( FILES ${INSTALL_PRIVATE_HEADERS} DESTINATION "include/mulle-container")
   install( FILES ${INSTALL_CMAKE_INCLUDES}  DESTINATION "include/mulle-container/cmake")

   # short cut out
   if( NOT COMPILE_PHASE AND NOT LINK_PHASE)
      return()
   endif()
endif()

if( LINK_PHASE)
   include( Dependencies OPTIONAL)
endif()

include( Library)

# use LIBRARY_COMPILE_TARGET for compiler options
target_compile_definitions( "${LIBRARY_COMPILE_TARGET}" PUBLIC)
target_compile_options( "${LIBRARY_COMPILE_TARGET}" PUBLIC)
if( NOT MSVC)
   target_compile_options( "${LIBRARY_COMPILE_TARGET}" PUBLIC -Wunused)
endif()

# use LIBRARY_LINK_TARGET for linker options
if( LINK_PHASE)
   target_link_options( "${LIBRARY_LINK_TARGET}" PUBLIC)
endif()

include( InstallLibrary)


if( LINK_PHASE)
   #
   # In a mulle-sde build the headers are installed into the SDK, so this
   # path is only for plain cmake consumers. The copying itself runs at the
   # start of the build regardless of where this include sits, the build
   # graph decides that, not the declaration order.
   #
   if( NOT USE_DEPENDENCY_DIR)
      include( AddSubdirectorySupport)
   endif()
   include( FindPackageSupport)
endif()

include( FinalOutput OPTIONAL)
