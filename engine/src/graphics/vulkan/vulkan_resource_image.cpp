
#include "real/core/types.hpp"
#include "real/resource/resource.hpp"
#include "vulkan_backend.hpp"
#include <real/resource/resource_image.hpp>
#include "vulkan_resource_image.hpp"
#include "vulkan_util.hpp"
#include "vulkan_renderer.hpp"

namespace real {

VulkanResourceImage::VulkanResourceImage(
    Instance *_instance, Optional<Path> _path,
    u32 width, u32 height, ColorFormat format, void *data) 
    : ResourceImage(_instance, _path, width, height, format, data) {

	renderer = (VulkanRenderer*)(instance->renderer.get());

	instance->log.trace("making image ");

	VkExtent3D drawImageExtent = {
		width,
		height,
		1
	};

	//hardcoding the draw format to 32 bit float
	imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	imageExtent = drawImageExtent;

	VkImageUsageFlags drawImageUsages{};
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VkImageCreateInfo rimg_info = vkutil::image_create_info(imageFormat, drawImageUsages, drawImageExtent);

	//for the draw image, we want to allocate it from gpu local memory
	VmaAllocationCreateInfo rimg_allocinfo = {};
	rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//allocate and create the image
	instance->log.trace("making image");
	vmaCreateImage(renderer->allocator, &rimg_info, &rimg_allocinfo, &image, &allocation, nullptr);
	instance->log.trace("making image");

	//build a image-view for the draw image to use for rendering
	VkImageViewCreateInfo rview_info = vkutil::imageview_create_info(
        imageFormat, image, VK_IMAGE_ASPECT_COLOR_BIT);

	VK_CHECK(vkCreateImageView(renderer->device, &rview_info, nullptr, &imageView));
}

VulkanResourceImage::~VulkanResourceImage() {
    // WindowBackendVulkan *window_backend = (WindowBackendVulkan*)instance->window->backend_handle();
    // ImageVulkan *image =  (ImageVulkan*)handle;

    vkDestroyImageView(renderer->device, imageView, nullptr);
	vmaDestroyImage(renderer->allocator, image, allocation);
}

ImageHandle VulkanResourceImage::get_handle() {
	return imageView;
}

}
