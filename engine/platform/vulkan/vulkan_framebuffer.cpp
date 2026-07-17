
#include "vulkan_framebuffer.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/graphics/framebuffer.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_resource_image.hpp"
#include "vulkan_util.hpp"
#include <memory>
#include <tracy/Tracy.hpp>

namespace real {

VulkanFramebuffer::VulkanFramebuffer(
        Instance *_instance, u32 _width, u32 _height,
        bool _depth, MultisamplingCount _count) 
            : Framebuffer(_instance, _width, _height, _depth, _count) {

    auto *resolve_image_ptr = new VulkanResourceImage(
        instance, width, height,
         ColorFormat::RGBA_FLOAT16, ImageFormat::RENDER_ATTACHMENT_COLOR, nullptr, 0);

    resolve_image = ResourceHandle<VulkanResourceImage>(
            instance->resource_database.get(), resolve_image_ptr, ResourceState::Loaded, UUID());    

    if(depth) {
        auto *depth_image_ptr = new VulkanResourceImage(
        instance, width, height,
         ColorFormat::DEPTH, ImageFormat::RENDER_ATTACHMENT_DEPTH,
          nullptr, 0, vkutil::MSAA_to_vulkan_counts(_count));

        depth_image = ResourceHandle<VulkanResourceImage>(
            instance->resource_database.get(), depth_image_ptr, ResourceState::Loaded, UUID());
    }

    if(_count != MultisamplingCount::One) {
        auto *msaa_color_image_ptr = new VulkanResourceImage(
        instance, width, height,
         ColorFormat::RGBA_FLOAT16, ImageFormat::RENDER_ATTACHMENT_COLOR,
          nullptr, 0, vkutil::MSAA_to_vulkan_counts(_count));

        msaa_color_image = ResourceHandle<VulkanResourceImage>(
            instance->resource_database.get(), msaa_color_image_ptr, ResourceState::Loaded, UUID());
    }
}

VulkanFramebuffer::~VulkanFramebuffer() {
    
}

ResourceHandle<ResourceImage> VulkanFramebuffer::get_depth_image() {
    return depth_image;
}

ResourceHandle<ResourceImage> VulkanFramebuffer::get_msaa_color_image() {
    return msaa_color_image;
}

ResourceHandle<ResourceImage> VulkanFramebuffer::get_color_resolve_image() {
    return resolve_image;
}

void VulkanFramebuffer::bind() {
}

void VulkanFramebuffer::clear_image(Color4 col) {
	ZoneScoped

    FrameDataVulkan &frame = ((VulkanRenderer*)instance->renderer.get())->get_current_frame();
    VkImageSubresourceRange range{};
	range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	range.baseMipLevel   = 0;
	range.levelCount     = 1;
	range.baseArrayLayer = 0;
	range.layerCount     = 1;
	msaa_color_image.get()->transition_image(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	VkClearColorValue clearColor = {{col.r, col.g, col.b, col.a}};
	vkCmdClearColorImage(
			frame.main_command_buffer, msaa_color_image.get()->image, msaa_color_image.get()->current_layout,
			&clearColor, 1, &range);
}

void VulkanFramebuffer::unbind() {
	ZoneScoped
		
    if(msaa != MultisamplingCount::One) {
        FrameDataVulkan &frame = ((VulkanRenderer*)instance->renderer.get())->get_current_frame();
        VkCommandBuffer cmd = frame.main_command_buffer;
        VulkanResourceImage *vk_render_image = (VulkanResourceImage*)msaa_color_image.get();
        VulkanResourceImage *vk_resolve_image = (VulkanResourceImage*)resolve_image.get();
        vk_render_image->transition_image(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        vk_resolve_image->transition_image(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkImageResolve resolveRegion{};
        resolveRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        resolveRegion.srcOffset = { 0, 0, 0 };
        resolveRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        resolveRegion.dstOffset = { 0, 0, 0 };
        resolveRegion.extent = { width, height, 1 };

        vkCmdResolveImage(cmd, 
                        vk_render_image->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                        vk_resolve_image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        1, &resolveRegion);
    }
}

UniquePointer<Framebuffer> Framebuffer::create(
        Instance *instance, u32 width, u32 height,
        bool depth, MultisamplingCount count) {

    return UniquePointer<Framebuffer>(
			&instance->engine_allocator,
			(Framebuffer*)instance->engine_allocator.allocate_object<VulkanFramebuffer>(instance, width, height, depth, count));
    // return std::make_unique<VulkanFramebuffer>(instance, width, height, depth, count);
}

}
