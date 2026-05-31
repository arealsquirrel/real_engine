
#include "vulkan_resource_mesh.hpp"
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/resource/resource_mesh.hpp"
#include "vulkan_resource_mesh.hpp"
#include "vulkan_renderer.hpp"

namespace real {

VulkanResourceMesh::VulkanResourceMesh(
		Game *_game,
		std::vector<uint32_t> indices,
		char *vertex_data, size_t size) 
	: ResourceMesh(_game, indices, vertex_data, size),
	renderer((VulkanRenderer*)game->renderer.get()) {

	const size_t vertexBufferSize = size;
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

	memcpy(data, vertex_data, vertexBufferSize);
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

	vkutil::destroy_buffer(renderer, staging);

	indices_count = indices.size();
	RL_LOG_INFO("done making mesh");
}

VulkanResourceMesh::~VulkanResourceMesh() {
	vkutil::destroy_buffer(renderer, vertexBuffer);
	vkutil::destroy_buffer(renderer, indexBuffer);
}

void VulkanResourceMesh::bind(FrameContext context) {
	/*
	FrameDataVulkan *frame = (FrameDataVulkan*)context;
	vkCmdBindIndexBuffer(
			frame->main_command_buffer,
			indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	*/
}

/*
void VulkanResourceMesh::draw(FrameContext context) {
	FrameDataVulkan *frame = (FrameDataVulkan*)context;
	vkCmdDrawIndexed(frame->main_command_buffer, indices_count, 1, 0, 0, 0);
}
*/

void VulkanResourceMesh::unbind(FrameContext context) {

}

}

