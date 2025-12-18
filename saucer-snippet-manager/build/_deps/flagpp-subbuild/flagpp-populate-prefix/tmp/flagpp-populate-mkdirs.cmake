# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/flagpp-src"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/flagpp-build"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/flagpp-subbuild/flagpp-populate-prefix"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/flagpp-subbuild/flagpp-populate-prefix/tmp"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/flagpp-subbuild/flagpp-populate-prefix/src/flagpp-populate-stamp"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/flagpp-subbuild/flagpp-populate-prefix/src"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/flagpp-subbuild/flagpp-populate-prefix/src/flagpp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/flagpp-subbuild/flagpp-populate-prefix/src/flagpp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/flagpp-subbuild/flagpp-populate-prefix/src/flagpp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
