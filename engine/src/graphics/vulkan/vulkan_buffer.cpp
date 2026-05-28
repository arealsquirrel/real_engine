
#include "vulkan_buffer.hpp"

namespace real {

VulkanMeshBuffer::VulkanMeshBuffer(
		VulkanRenderer *_renderer,
		std::vector<uint32_t> indices,
		std::vector<char*> vertex_data) 
	: renderer(_renderer) {

	const size_t vertexBufferSize = vertex_data.size();
	const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

	// GPUMeshBuffers newSurface;

	//create vertex buffer
	vertexBuffer = vkutil::create_buffer(renderer,
		vertexBufferSize,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

	//find the adress of the vertex buffer
	VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,.buffer = vertexBuffer.buffer };
	address = vkGetBufferDeviceAddress(renderer->device, &deviceAdressInfo);

	//create index buffer
	indexBuffer = vkutil::create_buffer(renderer,
		indexBufferSize,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

	vkutil::AllocatedBuffer staging = vkutil::create_buffer(
			renderer, vertexBufferSize + indexBufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	
	void* data = staging.info.pMappedData;

	memcpy(data, vertex_data.data(), vertexBufferSize);
	memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);

	vkutil::immediate_submit(
		renderer->imm_fence, renderer->imm_command_buffer,
		renderer->device, renderer->graphics_queue, [&](VkCommandBuffer cmd) {
		VkBufferCopy vertexCopy{ 0 };
		vertexCopy.dstOffset = 0;
		vertexCopy.srcOffset = 0;
		vertexCopy.size = vertexBufferSize;

		vkCmdCopyBuffer(cmd, staging.buffer, vertexBuffer.buffer, 1, &vertexCopy);

		VkBufferCopy indexCopy{ 0 };
		indexCopy.dstOffset = 0;
		indexCopy.srcOffset = vertexBufferSize;
		indexCopy.size = indexBufferSize;

		vkCmdCopyBuffer(cmd, staging.buffer, indexBuffer.buffer, 1, &indexCopy);
	});

	vkutil::destroy_buffer(staging);
}

VulkanMeshBuffer::~VulkanMeshBuffer() {

}

}

