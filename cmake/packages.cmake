
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

# size of this one does not matter because its not shipped
CPMAddPackage("gh:google/googletest@1.16.0")

# optimized and stripped size 50KiB
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
    GIT_TAG 1.0.1  # Specify a stable version or "master"
)
set(BUILD_SHARED_LIBS OFF)

CPMAddPackage(
    GIT_TAG v1.4.349
    GITHUB_REPOSITORY charles-lunarg/vk-bootstrap
    OPTIONS "-fPIC"
)

CPMAddPackage(
    Name VulkanMemoryAllocator
    GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    VERSION 3.1.0 # Use the latest stable version
    OPTIONS "VMA_BUILD_SAMPLES OFF" # Prevents building internal test apps
)

target_compile_options(vk-bootstrap PRIVATE -fPIC)

