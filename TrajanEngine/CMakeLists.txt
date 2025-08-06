cmake_minimum_required(VERSION 3.29)
project(TrajanEngine LANGUAGES C CXX)

# Require Graphics Pipelines
# TODO: Make these optional based on compilation targets
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)

# Engine Source Files
include(${CMAKE_CURRENT_SOURCE_DIR}/src/sources.cmake)

# Create Shared Library
add_library(TrajanEngine SHARED ${ENGINE_SOURCES})
target_compile_definitions(TrajanEngine PRIVATE TRAJANENGINE_EXPORTS)

# Vulkan-Specific Defines For vulkan-hpp
target_compile_definitions(TrajanEngine PUBLIC
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

# Engine Include Paths
target_include_directories(TrajanEngine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/src/components
            ${CMAKE_CURRENT_SOURCE_DIR}/src/core
            ${CMAKE_CURRENT_SOURCE_DIR}/src/opengl
            ${CMAKE_CURRENT_SOURCE_DIR}/src/systems
        PRIVATE
            ${Vulkan_INCLUDE_DIR}
            ${CMAKE_SOURCE_DIR}/external/glfw/include
            ${CMAKE_SOURCE_DIR}/external/imgui
            ${CMAKE_SOURCE_DIR}/external/imgui/backends
)

# External Libraries

## GLM <- Header
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/external/glm/glm)

## glad <- Static Library
add_library(glad STATIC
    ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c
)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/external/glad/include)

## imgui <- Static Library
add_library(imgui STATIC
        ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
        ${CMAKE_SOURCE_DIR}/external/imgui/imgui_demo.cpp
        ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
        ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
        ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp

        ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/external/imgui
        ${CMAKE_SOURCE_DIR}/external/imgui/backends
        ${CMAKE_SOURCE_DIR}/external/glfw/include
)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

# Link Libraries
target_link_libraries(TrajanEngine
    PRIVATE
        glfw
        glad
        glslang
        SPIRV
    PUBLIC
        Vulkan::Vulkan
        OpenGL::GL
        glm
        imgui
)
