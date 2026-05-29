
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
	"https://raw.githubusercontent.com/tinyobjloader/tinyobjloader/refs/heads/release/tiny_obj_loader.h"
	"${PROJECT_SOURCE_DIR}/vendor/tiny_obj_loader.hpp"
)

file(DOWNLOAD
	"https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h"
	"${PROJECT_SOURCE_DIR}/vendor/stb_image.h"
)

file(GLOB_RECURSE SRCS src/*.cpp vendor/*.cpp vendor/*.c)

configure_file(${PROJECT_SOURCE_DIR}/include/real/config/config.hpp.in
			   ${PROJECT_SOURCE_DIR}/include/real/config/config.hpp)

add_library(${PROJECT_NAME} SHARED ${SRCS})

target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden -fPIC)
target_compile_definitions(${PROJECT_NAME} PUBLIC SPIRV_REFLECT_USE_SYSTEM_SPIRV_H)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/vendor
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE GPUOpen::VulkanMemoryAllocator
	PUBLIC fmt
	PRIVATE Vulkan::Vulkan
	PRIVATE vk-bootstrap
	PUBLIC glfw
	PUBLIC imgui_cmake_wrapper
	PUBLIC glm::glm
)

