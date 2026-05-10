
cmake_minimum_required(VERSION 4.0)

project(real_engine LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("cmake/get_cpm.cmake")
include("cmake/packages.cmake")

enable_testing()


add_subdirectory(engine)
add_subdirectory(app)
add_subdirectory(tests)
