# Collect source files
# TODO: They do not recommend GLOB_RECURSE... fix this later
file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

# Include path to headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Create executable
add_executable(TrajanEditor ${EDITOR_SOURCES})
target_link_libraries(TrajanEditor PRIVATE TrajanEngine)

target_include_directories(TrajanEditor PRIVATE ${CMAKE_SOURCE_DIR}/external/glad/include)
target_link_libraries(TrajanEditor PRIVATE glad)

# TODO: copy only wanted headers into build
target_include_directories(TrajanEditor PRIVATE ${CMAKE_SOURCE_DIR}/TrajanEngine/src)