
#include "vulkan_buffer.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/buffer.hpp"
#include "vulkan_backend.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_resource_mesh.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>

namespace real {

VulkanUniformBuffer::VulkanUniformBuffer(Instance *_instance, size_t _size) 
    : UniformBuffer(_instance, _size), renderer((VulkanRenderer*)_instance->renderer.get()) {

    buffer = vkutil::create_buffer(
        renderer, _size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

    RL_LOG_INFO("{}", buffer.info.size);
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
    vkutil::destroy_buffer(renderer, buffer);
}

void *VulkanUniformBuffer::get_data() {
    return buffer.info.pMappedData;
}

BufferHandle VulkanUniformBuffer::get_handle() {
    return (BufferHandle)(this);
}

Unique<UniformBuffer> UniformBuffer::create(Instance *instance, size_t size) {
    return std::make_unique<VulkanUniformBuffer>(instance, size);
}

}
