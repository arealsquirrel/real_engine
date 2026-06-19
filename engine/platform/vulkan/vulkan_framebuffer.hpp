#ifndef REALLIB_VULKAN_FRAMEBUFFER
#define REALLIB_VULKAN_FRAMEBUFFER

#include "real/core/object.hpp"
#include "real/graphics/framebuffer.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include "vulkan_resource_image.hpp"
namespace real {

class VulkanFramebuffer : public Framebuffer {
RL_OBJECT(VulkanFramebuffer, Framebuffer)

public:
    VulkanFramebuffer(
        Instance *instance, u32 width, u32 height,
        bool depth=true, MultisamplingCount count=MultisamplingCount::One);

    ~VulkanFramebuffer();

    ResourceHandle<ResourceImage> get_depth_image() final override;
    ResourceHandle<ResourceImage> get_msaa_color_image() final override;
    ResourceHandle<ResourceImage> get_color_resolve_image() final override;

    void bind() final override;
    void clear_image(Color4 col={0.0f, 0.0f, 0.0f, 0.0f}) final override;
    void unbind() final override;

private:
    ResourceHandle<VulkanResourceImage> depth_image;
    ResourceHandle<VulkanResourceImage> msaa_color_image;
    ResourceHandle<VulkanResourceImage> resolve_image;
};

}

#endif