
#include "vulkan_buffer.hpp"
#include "real/container/ref.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/debug/timer.hpp"
#include "real/graphics/buffer.hpp"
#include "vulkan_backend.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_resource_mesh.hpp"
#include <cstring>
#include <memory>
#include <tracy/Tracy.hpp>
#include <vulkan/vulkan_core.h>

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

}
