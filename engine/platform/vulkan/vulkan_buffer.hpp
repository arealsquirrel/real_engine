#ifndef REALLIB_VULKAN_BUFFER_HPP
#define REALLIB_VULKAN_BUFFER_HPP

#include "real/core/object.hpp"
#include "real/graphics/buffer.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_renderpass_geometry.hpp"
#include <cstddef>
#include <vulkan/vulkan_core.h>
#include "vulkan_util.hpp"

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

class VulkanUniformBuffer : public UniformBuffer {
RL_OBJECT(VulkanUniformBuffer, UniformBuffer)

friend VulkanRenderPassGeometry;

public:
    VulkanUniformBuffer(Instance *_instance, size_t _size);
    ~VulkanUniformBuffer();

    void *get_data() override;
    BufferHandle get_handle() override;

    const vkutil::AllocatedBuffer &get_buffer() const { return buffer; }

private:
    vkutil::AllocatedBuffer buffer;
    VulkanRenderer *renderer;
};

class REALLIB_EXPORT VulkanIndexBuffer : public IndexBuffer {
RL_OBJECT(VulkanIndexBuffer, IndexBuffer)

public:
	VulkanIndexBuffer(Instance *_instance, size_t size);
	virtual ~VulkanIndexBuffer();

	void upload_data(std::vector<uint32_t> indices) override;

public:
	vkutil::AllocatedBuffer indexBuffer;
	VulkanRenderer *renderer;
};

class REALLIB_EXPORT VulkanVertexBuffer : public VertexBuffer {
RL_OBJECT(VulkanVertexBuffer, VertexBuffer)

public:
	VulkanVertexBuffer(Instance *_instance, size_t size);
	virtual ~VulkanVertexBuffer();

	void upload_data(char *data, size_t size) override;

public:
	vkutil::AllocatedBuffer vertexBuffer;
	VulkanRenderer *renderer;
	VkDeviceAddress address;
};


}

#endif
