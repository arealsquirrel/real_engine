
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include "vulkan_backend.hpp"
#include <real/resource/resource_image.hpp>
#include <vulkan/vulkan_core.h>
#include "vulkan_resource_image.hpp"
#include "vulkan_util.hpp"
#include "vulkan_renderer.hpp"

namespace real {

VulkanResourceImage::VulkanResourceImage(
		Game *_game,
		VkImage _image, VkImageView _view,
		VkExtent3D _extent, VkFormat _format,
		bool _internaly_managed)
	: ResourceImage(_game, _extent.width, _extent.height,
			ColorFormat::UNKNOWN, nullptr), internaly_managed(_internaly_managed),
		image(_image), imageView(_view), imageExtent(_extent), imageFormat(_format) {}

VulkanResourceImage::VulkanResourceImage(
    Game *_game,
    u32 width, u32 height, ColorFormat format, void *data, std::optional<Path> _path)  
    : ResourceImage(_game, width, height, format, data, _path), internaly_managed(false) {

	renderer = (VulkanRenderer*)(game->renderer.get());

	VkExtent3D drawImageExtent = {width, height, 1};

	VkImageCreateInfo rimg_info = {};
	VmaAllocationCreateInfo rimg_allocinfo = {};
	rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// hardcoding the draw format to 32 bit float
	switch (format) {
		case (ColorFormat::DEPTH): {
			imageFormat = VK_FORMAT_D32_SFLOAT;
			imageExtent = drawImageExtent;
			VkImageUsageFlags depthImageUsages{};
			depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			rimg_info = vkutil::image_create_info(imageFormat, depthImageUsages, drawImageExtent);
			VK_CHECK(vmaCreateImage(renderer->allocator, &rimg_info, &rimg_allocinfo, &image, &allocation, nullptr));
			VkImageViewCreateInfo rview_info = vkutil::imageview_create_info(imageFormat, image, VK_IMAGE_ASPECT_DEPTH_BIT);
			VK_CHECK(vkCreateImageView(renderer->device, &rview_info, nullptr, &imageView));
			RL_LOG_TRACE("Createing depth image");
			return;
		}
	
		case (ColorFormat::RGBA_FLOAT): {
			imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
			imageExtent = drawImageExtent;
			VkImageUsageFlags drawImageUsages{};
			drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
			drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			rimg_info = vkutil::image_create_info(imageFormat, drawImageUsages, drawImageExtent);
			VK_CHECK(vmaCreateImage(renderer->allocator, &rimg_info, &rimg_allocinfo, &image, &allocation, nullptr));
			VkImageViewCreateInfo rview_info = vkutil::imageview_create_info(imageFormat, image, VK_IMAGE_ASPECT_COLOR_BIT);
			VK_CHECK(vkCreateImageView(renderer->device, &rview_info, nullptr, &imageView));
			RL_LOG_TRACE("Creating RGBA float image");
			return;
		}
	default:
		RL_LOG_ERROR("color format not supported yet");
	}

}

VulkanResourceImage::~VulkanResourceImage() {
	if(internaly_managed == false) {
    	vkDestroyImageView(renderer->device, imageView, nullptr);
		vmaDestroyImage(renderer->allocator, image, allocation);
	}
}

void VulkanResourceImage::transition_image(FrameContext context, ImageFormat to) {
	FrameDataVulkan *data = (FrameDataVulkan*)context;
	

}

ImageHandle VulkanResourceImage::get_handle() {
	return imageView;
}

ResourceImage *ResourceImage::create(
		Game *_game,
		u32 width, u32 height, 
		ColorFormat format, void *data) {

	return new VulkanResourceImage(_game, width, height, format, data);
}

}
