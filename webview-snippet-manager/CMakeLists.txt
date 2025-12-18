cmake_minimum_required(VERSION 3.21)
project(webview-snippet-manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add webview library path
set(WEBVIEW_DIR "/home/randy/._____RANDY_____/REPOS/aria/docs/research/webview-lib")

# Find required packages for Linux
if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.1)
endif()

# Our executable
add_executable(snippet-manager
    src/main.cpp
    src/snippet_manager.cpp
)

# Include directories
target_include_directories(snippet-manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${WEBVIEW_DIR}/core/include
)

# Link libraries
if(UNIX AND NOT APPLE)
    target_include_directories(snippet-manager PRIVATE ${GTK3_INCLUDE_DIRS} ${WEBKIT2_INCLUDE_DIRS})
    target_link_libraries(snippet-manager PRIVATE ${GTK3_LIBRARIES} ${WEBKIT2_LIBRARIES})
elseif(APPLE)
    target_link_libraries(snippet-manager PRIVATE "-framework WebKit")
elseif(WIN32)
    # WebView2 will be linked automatically
endif()

# Copy frontend files to build directory
add_custom_command(TARGET snippet-manager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/frontend
        ${CMAKE_BINARY_DIR}/frontend
)

# Create snippets directory
add_custom_command(TARGET snippet-manager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_BINARY_DIR}/snippets
)
