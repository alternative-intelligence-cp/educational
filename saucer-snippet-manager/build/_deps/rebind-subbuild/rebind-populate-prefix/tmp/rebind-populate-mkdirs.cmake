# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/rebind-src"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/rebind-build"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/rebind-subbuild/rebind-populate-prefix"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/rebind-subbuild/rebind-populate-prefix/tmp"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/rebind-subbuild/rebind-populate-prefix/src/rebind-populate-stamp"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/rebind-subbuild/rebind-populate-prefix/src"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/rebind-subbuild/rebind-populate-prefix/src/rebind-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/rebind-subbuild/rebind-populate-prefix/src/rebind-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/rebind-subbuild/rebind-populate-prefix/src/rebind-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
