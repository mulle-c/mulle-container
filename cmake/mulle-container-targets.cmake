# Export mulle-container library target
add_library( mulle-container::mulle-container ALIAS ${LIBRARY_LINK_TARGET})

set_target_properties(${LIBRARY_LINK_TARGET} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${SOURCE_PUBLIC_INCLUDE_DIR}"
    INTERFACE_COMPILE_DEFINITIONS ""
)

function( mulle_target_link_library_with_whole_archive target)
    if(APPLE)
        target_link_libraries(${target} INTERFACE "-Wl,-all_load" ${ARGN})
    elseif(UNIX)
        target_link_libraries(${target} INTERFACE "-Wl,--whole-archive" ${ARGN} "-Wl,--no-whole-archive")
    elseif(MSVC)
        foreach(lib ${ARGN})
            target_link_libraries(${target} INTERFACE "-WHOLEARCHIVE:${lib}")
        endforeach()
    else()
        target_link_libraries(${target} INTERFACE ${ARGN})
    endif()
endfunction()

for( TMP_DEPENDENCY IN ALL_LOAD_DEPENDENCY_LIBRARIES)
   mulle_target_link_library_with_whole_archive( ${LIBRARY_LINK_TARGET}
                                                 ${TMP_DEPENDENCY}::${TMP_DEPENDENCY})
endfor()


for( TMP_LIBRARY IN DEPENDENCY_LIBRARIES)
   target_link_libraries(${LIBRARY_LINK_TARGET}
      INTERFACE
         ${TMP_LIBRARY}::${TMP_LIBRARY}
   )
endfor()

#
# Don't do optional dependencies
#

for( TMP_LIBRARY IN ALL_LOAD_OS_SPECIFIC_LIBRARIES)
    mulle_target_link_library_with_whole_archive(${LIBRARY_LINK_TARGET} ${TMP_LIBRARY})
endfor()


for( TMP_LIBRARY IN OS_SPECIFIC_LIBRARIES)
   target_link_libraries(${LIBRARY_LINK_TARGET}
      INTERFACE
         ${TMP_LIBRARY})
endfor()


# Mark as exported
set_property(GLOBAL APPEND PROPERTY mulle-container_EXPORTED_TARGETS ${LIBRARY_LINK_TARGET})