# Init
cmake_minimum_required(VERSION 3.29)
project(Trajan LANGUAGES C CXX)

# Establish C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Setup build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/lib)

# Need these for glslang
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)

# Subdirectories
add_subdirectory(external/glfw)
add_subdirectory(external/glslang)
add_subdirectory(TrajanEngine)
add_subdirectory(TrajanEditor)