#ifndef REALLIB_VULKAN_RESOURCE_MESH_HPP
#define REALLIB_VULKAN_RESOURCE_MESH_HPP

#include "real/core/game.hpp"
#include "real/core/object.hpp"
#include "real/resource/resource_mesh.hpp"
#include "vulkan_renderer.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace real {

namespace vkutil {

struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};

static inline AllocatedBuffer create_buffer(
		VulkanRenderer *renderer, size_t allocSize,
		VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {

	VkBufferCreateInfo bufferInfo = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
	bufferInfo.pNext = nullptr;
	bufferInfo.size = allocSize;
	bufferInfo.usage = usage;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = memoryUsage;
	vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocatedBuffer newBuffer;

	VK_CHECK(vmaCreateBuffer(renderer->allocator, &bufferInfo, &vmaallocInfo, &newBuffer.buffer, &newBuffer.allocation,
		&newBuffer.info));

	return newBuffer;
}

static inline void destroy_buffer(VulkanRenderer *renderer, const AllocatedBuffer& buffer) {
    vmaDestroyBuffer(renderer->allocator, buffer.buffer, buffer.allocation);
}

}

class VulkanResourceMesh : public ResourceMesh {
RL_OBJECT(VulkanResourceMesh, ResourceMesh)

public:
	VulkanResourceMesh(
			Instance *_instance,
			std::vector<uint32_t> indices,
			char *data, size_t size, std::map<StringHash, ResourceMesh::Mesh> meshes={}, bool is_static=true);

	~VulkanResourceMesh();

	void bind() final override;
	void unbind() final override;
	MeshAddress get_address() final override { return (char*)address; }
	void upload_vertex_data(char *vertex_data, u32 size) final override;
	// void upload_index_data(std::vector<u32> indices) final override;

public:
	vkutil::AllocatedBuffer vertexBuffer;
	std::optional<vkutil::AllocatedBuffer> indexBuffer {std::nullopt};
	VkDeviceAddress address;
	VulkanRenderer *renderer;
};

}

#endif
