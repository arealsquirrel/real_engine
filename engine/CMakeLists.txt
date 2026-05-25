
project (reallib VERSION 1.0.0)

file(DOWNLOAD 
     "https://raw.githubusercontent.com/KhronosGroup/SPIRV-Reflect/refs/heads/main/spirv_reflect.h"
	 "${PROJECT_SOURCE_DIR}/vendor/spirv_reflect.h"
)

file(DOWNLOAD 
     "https://raw.githubusercontent.com/KhronosGroup/SPIRV-Reflect/refs/heads/main/spirv_reflect.c"
     "${PROJECT_SOURCE_DIR}/vendor/spirv_reflect.cpp"
)

file(DOWNLOAD
	"https://raw.githubusercontent.com/syoyo/tinygltfloader/refs/heads/master/tiny_gltf_loader.h"
	"${PROJECT_SOURCE_DIR}/vendor/tiny_gltf_loader.h"
)

file(DOWNLOAD
	"https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h"
	"${PROJECT_SOURCE_DIR}/vendor/stb_image.h"
)

file(GLOB_RECURSE SRCS src/*.cpp vendor/*.cpp)

configure_file(${PROJECT_SOURCE_DIR}/config/config.hpp.in
			   ${PROJECT_SOURCE_DIR}/config/config.hpp)

add_library(${PROJECT_NAME} ${SRCS})

target_compile_definitions(${PROJECT_NAME} PUBLIC SPIRV_REFLECT_USE_SYSTEM_SPIRV_H)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/vendor
)

target_link_libraries(${PROJECT_NAME}
    fmt
	Vulkan::Vulkan
	vk-bootstrap
	GPUOpen::VulkanMemoryAllocator
	glfw
	imgui_cmake_wrapper
)

