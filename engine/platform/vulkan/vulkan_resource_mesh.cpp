
#include "vulkan_resource_mesh.hpp"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/resource/resource_mesh.hpp"
#include "vulkan_resource_mesh.hpp"
#include "vulkan_renderer.hpp"
#include <tracy/Tracy.hpp>

namespace real {

VulkanResourceMesh::VulkanResourceMesh(
		Instance *_instance,
		std::vector<uint32_t> indices,
		char *vertex_data, size_t size, std::map<StringHash, ResourceMesh::Mesh> meshes, bool _is_static) 
	: ResourceMesh(_instance, indices, vertex_data, size, meshes, _is_static),

	renderer((VulkanRenderer*)instance->renderer.get()) {

	ZoneScoped

	const size_t vertexBufferSize = size;
	const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

	vertexBuffer = vkutil::create_buffer(renderer,
		vertexBufferSize,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

	VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,.buffer = vertexBuffer.buffer };
	address = vkGetBufferDeviceAddress(renderer->device, &deviceAdressInfo);

	if(indices.empty() == false) {	
		indexBuffer = vkutil::create_buffer(renderer,
			indexBufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_GPU_ONLY);
		
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

			vkCmdCopyBuffer(cmd, staging.buffer, indexBuffer->buffer, 1, &indexCopy);
		});
		indices_count = indices.size();
		vkutil::destroy_buffer(renderer, staging);
		renderer->delete_queue.push_function([&](){
			vkutil::destroy_buffer(renderer, indexBuffer.value());
		});
	}

	if(vertex_data != nullptr) {
		vkutil::AllocatedBuffer staging = vkutil::create_buffer(
				renderer, size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		void* data = staging.info.pMappedData;
		memcpy(data, vertex_data, size);
		vkutil::immediate_submit(
			renderer->imm_fence, renderer->imm_command_buffer,
			renderer->device, renderer->graphics_queue, [&](VkCommandBuffer cmd) {
			VkBufferCopy vertexCopy{ 0 };
			vertexCopy.dstOffset = 0;
			vertexCopy.srcOffset = 0;
			vertexCopy.size = size;
			vkCmdCopyBuffer(cmd, staging.buffer, vertexBuffer.buffer, 1, &vertexCopy);
		});
		vkutil::destroy_buffer(renderer, staging);
		renderer->delete_queue.push_function([&](){
			vkutil::destroy_buffer(renderer, vertexBuffer);
		});
	}
}

void VulkanResourceMesh::upload_vertex_data(char *vertex_data, u32 size) {
	ZoneScoped

	if(size == 0)
		return;

	vkutil::AllocatedBuffer staging = vkutil::create_buffer(
			renderer, size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	
	void* data = staging.info.pMappedData;

	memcpy(data, vertex_data, size);

	VkBufferCopy vertexCopy{ 0 };
	vertexCopy.dstOffset = 0;
	vertexCopy.srcOffset = 0;
	vertexCopy.size = size;

	vkCmdCopyBuffer(renderer->get_current_frame().main_command_buffer, staging.buffer, vertexBuffer.buffer, 1, &vertexCopy);

	/*
	vkutil::immediate_submit(
		renderer->imm_fence, renderer->imm_command_buffer,
		renderer->device, renderer->graphics_queue, [&](VkCommandBuffer cmd) {

		VkBufferCopy vertexCopy{ 0 };
		vertexCopy.dstOffset = 0;
		vertexCopy.srcOffset = 0;
		vertexCopy.size = size;

		vkCmdCopyBuffer(cmd, staging.buffer, vertexBuffer.buffer, 1, &vertexCopy);
	});
	*/

	vkutil::destroy_buffer(renderer, staging);
}

VulkanResourceMesh::~VulkanResourceMesh() {
	ZoneScoped
}

void VulkanResourceMesh::bind() {
}

void VulkanResourceMesh::unbind() {
}

UniquePointer<ResourceMesh> ResourceMesh::create(
		Instance *instance, std::vector<uint32_t> indices,
		char *data, size_t size, std::map<StringHash, ResourceMesh::Mesh> meshes, bool is_static) {

    return UniquePointer<ResourceMesh>(
			&instance->engine_allocator,
			(ResourceMesh*)instance->engine_allocator.allocate_object<VulkanResourceMesh>(
				instance, indices, data, size, meshes, is_static));
}

}

