
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

# size of this one does not matter because its not shipped
CPMAddPackage("gh:google/googletest@1.16.0")

# optimized and stripped size 50KiB
set(FMT_TEST OFF CACHE BOOL "Disable fmt tests")
CPMAddPackage(
	  NAME fmt
	  GIT_TAG 12.1.0
	  GITHUB_REPOSITORY fmtlib/fmt
)

CPMAddPackage("gh:charles-lunarg/vk-bootstrap@1.4.349")

# Vulkan-Headers defines a header target but not one for the module.
# fucking huge
CPMAddPackage(
    Name VulkanMemoryAllocator
    GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    VERSION 3.1.0 # Use the latest stable version
    OPTIONS
        "VMA_BUILD_SAMPLES OFF" # Prevents building internal test apps
)

