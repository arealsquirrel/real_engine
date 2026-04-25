
cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

include("cmake/get_cpm.cmake")
include("cmake/packages.cmake")

project(real_engine LANGUAGES CXX)

add_subdirectory(engine)
add_subdirectory(app)
add_subdirectory(tests)
