
#include "real/core/types.hpp"
#include "real/resource/resource.hpp"
#include "vulkan_backend.hpp"
#include <real/resource/resource_image.hpp>
#include <utility>
#include "vulkan_resource_image.hpp"
#include "vulkan_util.hpp"
#include "vulkan_renderer.hpp"

namespace real {

VulkanResourceImage::VulkanResourceImage(Instance *_instance,
		VkImage _image, VkImageView _view,
		VkExtent3D _extent, VkFormat _format,
		bool _internaly_managed)
	: ResourceImage(instance, _extent.width, _extent.height,
			ColorFormat::UNKNOWN, nullptr), internaly_managed(_internaly_managed),
		image(_image), imageView(_view), imageExtent(_extent), imageFormat(_format) {}

VulkanResourceImage::VulkanResourceImage(
    Instance *_instance,
    u32 width, u32 height, ColorFormat format, void *data, std::optional<Path> _path)  
    : ResourceImage(_instance, width, height, format, data, _path), internaly_managed(false) {

	renderer = (VulkanRenderer*)(instance->renderer.get());

	VkExtent3D drawImageExtent = {
		width,
		height,
		1
	};

	// hardcoding the draw format to 32 bit float
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
	vmaCreateImage(renderer->allocator, &rimg_info, &rimg_allocinfo, &image, &allocation, nullptr);

	//build a image-view for the draw image to use for rendering
	VkImageViewCreateInfo rview_info = vkutil::imageview_create_info(
        imageFormat, image, VK_IMAGE_ASPECT_COLOR_BIT);

	VK_CHECK(vkCreateImageView(renderer->device, &rview_info, nullptr, &imageView));
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
		Instance *_instance,
		u32 width, u32 height, ColorFormat format, void *data) {

	return new VulkanResourceImage(_instance, width, height, format, data);
}

}
