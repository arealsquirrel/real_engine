
project (reallib VERSION 1.0.0)

file(GLOB_RECURSE ENGINE_SRCS src/*.cpp external/*.cpp external/*.c)

configure_file(${PROJECT_SOURCE_DIR}/include/real/config/config.hpp.in
			   ${PROJECT_SOURCE_DIR}/include/real/config/config.hpp)

if("${REAL_RENDER_BACKEND}" STREQUAL "vulkan")
	file(GLOB_RECURSE VULKAN_SRCS platform/vulkan/*.cpp)
	list(APPEND ENGINE_SRCS ${VULKAN_SRCS})
endif()

if("${REAL_RENDER_BACKEND}" STREQUAL "opengl")
	message(ERROR "we dont have anything for opengl")
	file(GLOB_RECURSE OPENGL_SRCS platform/opengl/*.cpp)
	list(APPEND ENGINE_SRCS ${OPENGL_SRCS})
endif()

add_library(${PROJECT_NAME} SHARED ${ENGINE_SRCS})

target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden -fPIC)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/external
)

if("${REAL_RENDER_BACKEND}" STREQUAL "vulkan")
	target_compile_definitions(${PROJECT_NAME} PUBLIC SPIRV_REFLECT_USE_SYSTEM_SPIRV_H)

	target_link_libraries(${PROJECT_NAME}
		PRIVATE GPUOpen::VulkanMemoryAllocator
		PRIVATE Vulkan::Vulkan
		PRIVATE vk-bootstrap
		PUBLIC fmt
		PUBLIC glfw
		PUBLIC imgui_cmake_wrapper
		PUBLIC glm::glm
		PUBLIC EnTT::EnTT
	)

elseif("${REAL_RENDER_BACKEND}" STREQUAL "opengl")
	target_link_libraries(${PROJECT_NAME}
		PRIVATE GPUOpen::VulkanMemoryAllocator
		PRIVATE Vulkan::Vulkan
		PRIVATE vk-bootstrap
		PUBLIC fmt
		PUBLIC glfw
		PUBLIC imgui_cmake_wrapper
		PUBLIC glm::glm
		PUBLIC EnTT::EnTT
	)
endif()
