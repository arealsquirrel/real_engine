
find_package(glfw3 REQUIRED)
# find_package(slang REQUIRED)
set(IMGUI_PLATFORM_BACKEND "glfw")

message(STATUS "Downloading generic dependencies")

CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG v1.17.0
)

set(FMT_TEST OFF CACHE BOOL "Disable fmt tests")
set(BUILD_SHARED_LIBS ON)
CPMAddPackage(
	  NAME fmt
	  GIT_TAG 12.1.0
	  GITHUB_REPOSITORY fmtlib/fmt
      OPTIONS "-DBUILD_SHARED_LIBS"
)

CPMAddPackage(
    NAME entt
    GITHUB_REPOSITORY skypjack/entt
    GIT_TAG v3.16.0
)

CPMAddPackage(
  NAME nlohmann_json
  VERSION 3.7.3
  URL https://github.com/nlohmann/json/releases/download/v3.7.3/include.zip
  URL_HASH SHA256=87b5884741427220d3a33df1363ae0e8b898099fbc59f1c451113f6732891014
)

if (nlohmann_json_ADDED)
  add_library(nlohmann_json INTERFACE IMPORTED)
  target_include_directories(nlohmann_json INTERFACE ${nlohmann_json_SOURCE_DIR}/include)
endif()

FetchContent_Declare(
    tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG        v0.13.1 # Replace with your preferred stable release tag
)

FetchContent_MakeAvailable(tracy)

set(BUILD_SHARED_LIBS OFF)

file(DOWNLOAD 
     "https://raw.githubusercontent.com/KhronosGroup/SPIRV-Reflect/refs/heads/main/spirv_reflect.h"
	 "${PROJECT_SOURCE_DIR}/external/spirv_reflect.h"
)

file(DOWNLOAD 
     "https://raw.githubusercontent.com/stdbug/argparse/refs/heads/master/argparse/argparse.h"
	 "${PROJECT_SOURCE_DIR}/external/argparse.h"
)

file(DOWNLOAD 
     "https://raw.githubusercontent.com/KhronosGroup/SPIRV-Reflect/refs/heads/main/spirv_reflect.c"
     "${PROJECT_SOURCE_DIR}/external/spirv_reflect.cpp"
)

file(DOWNLOAD
	"https://raw.githubusercontent.com/tinyobjloader/tinyobjloader/refs/heads/release/tiny_obj_loader.h"
	"${PROJECT_SOURCE_DIR}/external/tiny_obj_loader.hpp"
)

file(DOWNLOAD
	"https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h"
	"${PROJECT_SOURCE_DIR}/external/stb_image.h"
)

file(DOWNLOAD
	"https://raw.githubusercontent.com/iboB/picobench/refs/heads/master/include/picobench/picobench.hpp"
	"${PROJECT_SOURCE_DIR}/external/picobench.h"
)


if("${REAL_RENDER_BACKEND}" STREQUAL "vulkan")
    set(IMGUI_RENDERER_BACKEND "vulkan")
    find_package(Vulkan REQUIRED)
    message(STATUS "Downloading Vulkan specific dependencies")
    CPMAddPackage(
        GIT_TAG v1.4.349
        GITHUB_REPOSITORY charles-lunarg/vk-bootstrap
        OPTIONS "-fPIC"
    )

    target_compile_options(vk-bootstrap PRIVATE -fPIC)

    CPMAddPackage(
        Name VulkanMemoryAllocator
        GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        VERSION 3.1.0 # Use the latest stable version
        OPTIONS "VMA_BUILD_SAMPLES OFF" # Prevents building internal test apps
    )
endif()

if("${REAL_RENDER_BACKEND}" STREQUAL "opengl")
    set(IMGUI_RENDERER_BACKEND "opengl")
    find_package(OpenGL REQUIRED)
endif()

