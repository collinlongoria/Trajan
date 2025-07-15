# Init
cmake_minimum_required(VERSION 3.29)

project(Trajan LANGUAGES CXX)

# Establish C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Setup build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Build/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Build/lib)

# Subdirectories
add_subdirectory(external/glfw)
add_subdirectory(TrajanEngine)
add_subdirectory(TrajanEditor)