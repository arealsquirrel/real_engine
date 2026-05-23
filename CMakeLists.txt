
cmake_minimum_required(VERSION 4.0)

project(real_engine LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(IMGUI_RENDERER_BACKEND "vulkan")
set(IMGUI_PLATFORM_BACKEND "glfw")

include("cmake/get_cpm.cmake")
include("cmake/packages.cmake")
include("cmake/imgui_cmake.cmake")

enable_testing()

add_compile_options(-fsanitize=address -fno-omit-frame-pointer -g)
add_link_options(-fsanitize=address)

add_subdirectory(engine)
add_subdirectory(app)
add_subdirectory(tests)

add_custom_target(
    Shaders
    COMMAND make
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/engine/resources/"
    COMMENT "compiling engine resources"
)

