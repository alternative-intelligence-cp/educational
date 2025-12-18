#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cr::coco" for configuration "Release"
set_property(TARGET cr::coco APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(cr::coco PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/coco-3.2.2/libcoco.a"
  )

list(APPEND _cmake_import_check_targets cr::coco )
list(APPEND _cmake_import_check_files_for_cr::coco "${_IMPORT_PREFIX}/lib/coco-3.2.2/libcoco.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
