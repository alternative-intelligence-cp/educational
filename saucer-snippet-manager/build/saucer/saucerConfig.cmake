include(CMakeFindDependencyMacro)

string(REGEX MATCHALL "[^;]+" SEPARATE_DEPENDENCIES "lockpp 3.2.0;coco 3.2.2;rebind 5.3.1;ereignis 6.2.0;flagpp 3.1.0;polo 1.0.1;saucer-fill 1.3.0;glaze 6.0.3")

foreach(dependency ${SEPARATE_DEPENDENCIES})
  string(REPLACE " " ";" args "${dependency}")
  find_dependency(${args})
endforeach()

include("${CMAKE_CURRENT_LIST_DIR}/saucerTargets.cmake")
