
#include "vulkan_buffer.hpp"
#include "real/container/ref.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/buffer.hpp"
#include "vulkan_backend.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_resource_mesh.hpp"
#include <cstring>
#include <tracy/Tracy.hpp>
#include <vulkan/vulkan_core.h>
#include <real/graphics/buffer.hpp>

namespace real {

VulkanUniformBuffer::VulkanUniformBuffer(Instance *_instance, size_t _size) 
    : UniformBuffer(_instance, _size), renderer((VulkanRenderer*)_instance->renderer.get()) {

	ZoneScoped

    buffer = vkutil::create_buffer(
        renderer, _size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

    memset(buffer.info.pMappedData, 0, size);
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
	ZoneScoped

	vkutil::destroy_buffer(renderer, buffer);
}

void *VulkanUniformBuffer::get_data() {
    return buffer.info.pMappedData;
}

BufferHandle VulkanUniformBuffer::get_handle() {
    return (BufferHandle)(this);
}

UniquePointer<UniformBuffer> UniformBuffer::create(Instance *instance, size_t size) {
    return UniquePointer<UniformBuffer>(
			&instance->engine_allocator,
			(UniformBuffer*)instance->engine_allocator.allocate_object<VulkanUniformBuffer>(instance, size));
}

VulkanIndexBuffer::VulkanIndexBuffer(Instance *_instance, size_t size)
	: IndexBuffer(_instance, size), renderer((VulkanRenderer*)_instance->renderer.get()) {

	indexBuffer = vkutil::create_buffer(
		renderer, size,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

	renderer->tag_object(VK_OBJECT_TYPE_BUFFER, (uint64_t)indexBuffer.buffer, "Index Buffer");
}

VulkanIndexBuffer::~VulkanIndexBuffer() {
	vkutil::destroy_buffer(renderer, indexBuffer);
}

void VulkanIndexBuffer::upload_data(std::vector<uint32_t> indices) {
	u32 indexBufferSize = indices.size() * sizeof(u32);
	vkutil::AllocatedBuffer staging = vkutil::create_buffer(
			renderer, indexBufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data = staging.info.pMappedData;
	memcpy((char*)data, indices.data(), indexBufferSize);
	vkutil::immediate_submit(
		renderer->imm_fence, renderer->imm_command_buffer,
		renderer->device, renderer->graphics_queue, [&](VkCommandBuffer cmd) {
	
		VkBufferCopy indexCopy{ 0 };
		indexCopy.dstOffset = 0;
		indexCopy.srcOffset = 0;
		indexCopy.size = indexBufferSize;

		vkCmdCopyBuffer(cmd, staging.buffer, indexBuffer.buffer, 1, &indexCopy);
	});

	vkutil::destroy_buffer(renderer, staging);
}

VulkanVertexBuffer::VulkanVertexBuffer(Instance *_instance, size_t _size) 
	: VertexBuffer(_instance, _size), renderer((VulkanRenderer*)_instance->renderer.get()) {

	vertexBuffer = vkutil::create_buffer(renderer, size,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

	VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = vertexBuffer.buffer };
	address = vkGetBufferDeviceAddress(renderer->device, &deviceAdressInfo);

	renderer->tag_object(VK_OBJECT_TYPE_BUFFER, (uint64_t)vertexBuffer.buffer, "Vertex Buffer");
}

VulkanVertexBuffer::~VulkanVertexBuffer() {
	vkutil::destroy_buffer(renderer, vertexBuffer);
}

void VulkanVertexBuffer::upload_data(char *vertex_data, size_t upload_size) {
	vkutil::AllocatedBuffer staging = vkutil::create_buffer(
			renderer, upload_size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data = staging.info.pMappedData;
	memcpy(data, vertex_data, upload_size);
	vkutil::immediate_submit(
		renderer->imm_fence, renderer->imm_command_buffer,
		renderer->device, renderer->graphics_queue, [&](VkCommandBuffer cmd) {
		VkBufferCopy vertexCopy{ 0 };
		vertexCopy.dstOffset = 0;
		vertexCopy.srcOffset = 0;
		vertexCopy.size = upload_size;
		vkCmdCopyBuffer(cmd, staging.buffer, vertexBuffer.buffer, 1, &vertexCopy);
	});
	vkutil::destroy_buffer(renderer, staging);
}

UniquePointer<VertexBuffer> VertexBuffer::create(Instance *instance, size_t size) {
    return UniquePointer<VertexBuffer>(
			&instance->engine_allocator,
			(VertexBuffer*)instance->engine_allocator.allocate_object<VulkanVertexBuffer>(
				instance, size));
}

UniquePointer<IndexBuffer> IndexBuffer::create(Instance *instance, size_t size) {
    return UniquePointer<IndexBuffer>(
			&instance->engine_allocator,
			(IndexBuffer*)instance->engine_allocator.allocate_object<VulkanIndexBuffer>(
				instance, size));
}

}

