
cmake_minimum_required(VERSION 4.0)

project(real_engine LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")      # Debug flags
set(CMAKE_CXX_FLAGS_MINSIZEREL "-g -Oz")      # Debug flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG") # Release flags

set(IMGUI_RENDERER_BACKEND "vulkan")
set(IMGUI_PLATFORM_BACKEND "glfw")

include("cmake/get_cpm.cmake")
include("cmake/packages.cmake")
include("cmake/imgui_cmake.cmake")

enable_testing()

add_subdirectory(engine)
add_subdirectory(app)
add_subdirectory(tests)
add_subdirectory(runtime)
add_subdirectory(editor)

add_custom_target(
    shaders
    COMMAND make
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/engine/resources/"
    COMMENT "compiling engine resources"
)

