# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-embed-src"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-embed-build"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-embed-subbuild/saucer-embed-populate-prefix"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-embed-subbuild/saucer-embed-populate-prefix/tmp"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-embed-subbuild/saucer-embed-populate-prefix/src/saucer-embed-populate-stamp"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-embed-subbuild/saucer-embed-populate-prefix/src"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-embed-subbuild/saucer-embed-populate-prefix/src/saucer-embed-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-embed-subbuild/saucer-embed-populate-prefix/src/saucer-embed-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/saucer-embed-subbuild/saucer-embed-populate-prefix/src/saucer-embed-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
