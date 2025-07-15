# Require Vulkan
find_package(Vulkan REQUIRED)

# Collect source files
# TODO: They do not recommend GLOB_RECURSE... fix this later
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

# Include path to headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Include Vulkan headers
include_directories(TrajanEngine PRIVATE ${Vulkan_INCLUDE_DIR})

# Create dynamic lib
add_library(TrajanEngine SHARED ${ENGINE_SOURCES})
target_compile_definitions(TrajanEngine PRIVATE TRAJANENGINE_EXPORTS)

target_compile_definitions(TrajanEngine PUBLIC
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

# Link external libraries
target_link_libraries(TrajanEngine PRIVATE glfw)

target_link_libraries(TrajanEngine PUBLIC Vulkan::Vulkan)

target_include_directories(TrajanEngine PRIVATE ${CMAKE_SOURCE_DIR}/external/glfw/include)