
find_package(glfw3 REQUIRED)
find_package(slang REQUIRED)
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
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 1.0.3  # Specify a stable version or "master"
)

CPMAddPackage(
    NAME entt
    GITHUB_REPOSITORY skypjack/entt
    GIT_TAG v3.16.0  # Specify a stable version or "master"
)

set(BUILD_SHARED_LIBS OFF)

file(DOWNLOAD 
     "https://raw.githubusercontent.com/KhronosGroup/SPIRV-Reflect/refs/heads/main/spirv_reflect.h"
	 "${PROJECT_SOURCE_DIR}/engine/external/spirv_reflect.h"
)

file(DOWNLOAD 
     "https://raw.githubusercontent.com/stdbug/argparse/refs/heads/master/argparse/argparse.h"
	 "${PROJECT_SOURCE_DIR}/engine/external/argparse.h"
)

file(DOWNLOAD 
     "https://raw.githubusercontent.com/KhronosGroup/SPIRV-Reflect/refs/heads/main/spirv_reflect.c"
     "${PROJECT_SOURCE_DIR}/engine/external/spirv_reflect.cpp"
)

file(DOWNLOAD
	"https://raw.githubusercontent.com/tinyobjloader/tinyobjloader/refs/heads/release/tiny_obj_loader.h"
	"${PROJECT_SOURCE_DIR}/engine/external/tiny_obj_loader.hpp"
)

file(DOWNLOAD
	"https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h"
	"${PROJECT_SOURCE_DIR}/engine/external/stb_image.h"
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

