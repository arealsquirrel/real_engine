
# optimized and stripped size 50KiB
CPMAddPackage(
	  NAME fmt
	  GIT_TAG 12.1.0
	  GITHUB_REPOSITORY fmtlib/fmt
)

# size of this one does not matter because its not shipped
CPMAddPackage("gh:google/googletest@1.16.0")


# Vulkan-Headers defines a header target but not one for the module.
# fucking huge
CPMAddPackage(
  NAME Vulkan-Headers
  GITHUB_REPOSITORY KhronosGroup/Vulkan-Headers
  GIT_TAG v1.3.269
)
if(Vulkan-Headers_ADDED)
  add_library(Vulkan-Module)
  target_sources(Vulkan-Module
    PUBLIC FILE_SET CXX_MODULES
    BASE_DIRS "${Vulkan-Headers_SOURCE_DIR}/include"
    FILES "${Vulkan-Headers_SOURCE_DIR}/include/vulkan/vulkan.cppm"
  )
  target_compile_definitions(Vulkan-Module PUBLIC
    # Your options here, project-dependent:
    # https://github.com/KhronosGroup/Vulkan-Hpp#configuration-options
  )
  target_link_libraries(Vulkan-Module PUBLIC Vulkan-Headers)
endif()
