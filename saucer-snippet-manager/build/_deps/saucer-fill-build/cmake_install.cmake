# Install script for directory: /home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-fill-src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "saucer-fill_Development" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/saucer-fill-1.3.0/saucer-fillTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/saucer-fill-1.3.0/saucer-fillTargets.cmake"
         "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-fill-build/CMakeFiles/Export/bc70c55962c15a695db7a867921b12b1/saucer-fillTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/saucer-fill-1.3.0/saucer-fillTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/saucer-fill-1.3.0/saucer-fillTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/saucer-fill-1.3.0" TYPE FILE FILES "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-fill-build/CMakeFiles/Export/bc70c55962c15a695db7a867921b12b1/saucer-fillTargets.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "saucer-fill_Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/saucer-fill-1.3.0" TYPE FILE FILES
    "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-fill-build/saucer-fillConfigVersion.cmake"
    "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-fill-build/saucer-fillConfig.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "saucer-fill_Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/saucer-fill-1.3.0" TYPE DIRECTORY FILES "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-fill-src/include/" FILES_MATCHING REGEX "/[^/]*$")
endif()

