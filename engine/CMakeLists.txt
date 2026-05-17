
project (reallib VERSION 1.0.0)

file(GLOB_RECURSE SRCS src/*.cpp)

configure_file(${PROJECT_SOURCE_DIR}/config/config.hpp.in
			   ${PROJECT_SOURCE_DIR}/config/config.hpp)

add_library(${PROJECT_NAME} ${SRCS})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    fmt
	Vulkan::Vulkan
	vk-bootstrap
	GPUOpen::VulkanMemoryAllocator
	glfw
)
