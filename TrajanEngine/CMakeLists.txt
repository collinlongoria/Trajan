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

# Create executable
add_executable(TrajanEngine ${ENGINE_SOURCES})

# Link external libraries
target_link_libraries(TrajanEngine PRIVATE glfw Vulkan::Vulkan)
target_include_directories(TrajanEngine PRIVATE ${CMAKE_SOURCE_DIR}/external/glfw/include)