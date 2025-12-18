cmake_minimum_required(VERSION 3.21)
project(snippet-manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add saucer from known location
set(SAUCER_DIR "/home/randy/._____RANDY_____/REPOS/aria/docs/research/saucer-master")

if(NOT EXISTS ${SAUCER_DIR})
    message(FATAL_ERROR "Saucer not found at ${SAUCER_DIR}")
endif()

add_subdirectory(${SAUCER_DIR} saucer)

# Our executable
add_executable(snippet-manager
    src/main.cpp
    src/snippet_manager.cpp
)

target_include_directories(snippet-manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(snippet-manager PRIVATE
    saucer::saucer
)

# Copy frontend files to build directory
add_custom_command(TARGET snippet-manager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/frontend
        $<TARGET_FILE_DIR:snippet-manager>/frontend
)

# Create snippets directory
add_custom_command(TARGET snippet-manager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:snippet-manager>/snippets
)
