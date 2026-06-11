#ifndef REALLIB_VULKAN_BUFFER_HPP
#define REALLIB_VULKAN_BUFFER_HPP

#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include "real/graphics/buffer.hpp"
#include "real/graphics/renderer.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_renderpass_geometry.hpp"
#include "vulkan_resource_mesh.hpp"
#include <cstddef>
#include <vulkan/vulkan_core.h>

namespace real {

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

}

#endif