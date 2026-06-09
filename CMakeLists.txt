
cmake_minimum_required(VERSION 4.0)

project(real_engine LANGUAGES CXX C)

# --------------- OPTIONS --------------- #
option(REAL_BUILD_TESTS "Build the projects test" ON)
option(REAL_BUILD_DOCS "Build docs for the engine" ON)
option(REAL_ADDRESS_SANITIZER "Build with address sanitizers" OFF)
set(REAL_RENDER_BACKEND "vulkan" CACHE STRING "Select the graphics backend")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------- BUILD TYPES --------------- #
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")      # Debug flags
set(CMAKE_CXX_FLAGS_MINSIZEREL "-g -Oz")      # Debug flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG") # Release flags

include("cmake/docs.cmake")
include("cmake/get_cpm.cmake")
include("cmake/packages.cmake")
include("cmake/imgui_cmake.cmake")

if(REAL_BUILD_TESTS)
  enable_testing()
endif()

if(REAL_ADDRESS_SANITIZER)
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

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
