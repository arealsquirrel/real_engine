
#include "real/core/types.hpp"
#include "real/resource/resource.hpp"
#include "vulkan_backend.hpp"
#include <real/resource/resource_image.hpp>

namespace real {

ResourceImage::ResourceImage(
    Instance *_instance, Optional<Path> _path,
    u32 width, u32 height, ColorFormat format, void *data) 
    : Resource(_instance, _path) {

    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)instance->window->backend_handle();
    ImageVulkan *image = new ImageVulkan;

    //draw image size will match the window
	VkExtent3D drawImageExtent = {
		width,
		height,
		1
	};

	//hardcoding the draw format to 32 bit float
	image->imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	image->imageExtent = drawImageExtent;

	VkImageUsageFlags drawImageUsages{};
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VkImageCreateInfo rimg_info = vkutil::image_create_info(image->imageFormat, drawImageUsages, drawImageExtent);

	//for the draw image, we want to allocate it from gpu local memory
	VmaAllocationCreateInfo rimg_allocinfo = {};
	rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//allocate and create the image
	vmaCreateImage(window_backend->allocator, &rimg_info, &rimg_allocinfo, &image->image, &image->allocation, nullptr);

	//build a image-view for the draw image to use for rendering
	VkImageViewCreateInfo rview_info = vkutil::imageview_create_info(
        image->imageFormat, image->image, VK_IMAGE_ASPECT_COLOR_BIT);

	VK_CHECK(vkCreateImageView(window_backend->device, &rview_info, nullptr, &image->imageView));

    handle = image;
}

ResourceImage::~ResourceImage() {
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)instance->window->backend_handle();
    ImageVulkan *image =  (ImageVulkan*)handle;

    vkDestroyImageView(window_backend->device, image->imageView, nullptr);
	vmaDestroyImage(window_backend->allocator, image->image, image->allocation);

    delete (ImageVulkan*)handle;
}

}
