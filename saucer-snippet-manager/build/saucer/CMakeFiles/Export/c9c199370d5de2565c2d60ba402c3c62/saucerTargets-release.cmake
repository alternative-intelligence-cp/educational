#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "saucer::saucer" for configuration "Release"
set_property(TARGET saucer::saucer APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(saucer::saucer PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/saucer-8.0.0/libsaucer.a"
  )

list(APPEND _cmake_import_check_targets saucer::saucer )
list(APPEND _cmake_import_check_files_for_saucer::saucer "${_IMPORT_PREFIX}/lib/saucer-8.0.0/libsaucer.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
