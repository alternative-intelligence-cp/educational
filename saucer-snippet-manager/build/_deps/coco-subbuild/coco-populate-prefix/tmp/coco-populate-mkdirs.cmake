# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/coco-src"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/coco-build"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/coco-subbuild/coco-populate-prefix"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/coco-subbuild/coco-populate-prefix/tmp"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/coco-subbuild/coco-populate-prefix/src/coco-populate-stamp"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/coco-subbuild/coco-populate-prefix/src"
  "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/coco-subbuild/coco-populate-prefix/src/coco-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/coco-subbuild/coco-populate-prefix/src/coco-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/randy/._____RANDY_____/REPOS/educational/saucer-snippet-manager/build/_deps/coco-subbuild/coco-populate-prefix/src/coco-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
