
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include "vulkan_backend.hpp"
#include <real/resource/resource_image.hpp>
#include <vulkan/vulkan_core.h>
#include "vulkan_resource_image.hpp"
#include "vulkan_resource_mesh.hpp"
#include "vulkan_util.hpp"
#include "vulkan_renderer.hpp"

namespace real {

VulkanResourceImage::VulkanResourceImage(
    Instance *_instance,
    u32 width, u32 height, 
	ColorFormat _cformat, ImageFormat _iformat,
	void *data, int mips, VkSampleCountFlagBits _samples)
    : ResourceImage(_instance, width, height, _cformat, _iformat, data),
	  samples(_samples) {
	
	RL_INSTRUMENT_FUNCTION
	renderer = (VulkanRenderer*)(instance->renderer.get());

	VkExtent3D drawImageExtent = {width, height, 1};

	VkImageCreateInfo rimg_info = {};
	VmaAllocationCreateInfo rimg_allocinfo = {};
	VkImageUsageFlags imageUsages = {};
	VkImageAspectFlags aspect = 0;
	rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	imageExtent = drawImageExtent;


	// hardcoding the draw format to 32 bit float
	switch (cformat) {
		case (ColorFormat::DEPTH): {
			RL_LOG_TRACE("strait up depthing it rn");
			imageFormat = VK_FORMAT_D32_SFLOAT;
			imageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageUsages |= VK_IMAGE_USAGE_SAMPLED_BIT;
			rimg_info = vkutil::image_create_info(imageFormat, imageUsages, drawImageExtent);
			aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
			break;
		}
	
		case (ColorFormat::RGBA_FLOAT16): {
			RL_LOG_TRACE("Coloring it rn");
			imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
			imageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			imageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			if(samples == VK_SAMPLE_COUNT_1_BIT)
				imageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
			imageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			imageUsages |= VK_IMAGE_USAGE_SAMPLED_BIT;
			rimg_info = vkutil::image_create_info(imageFormat, imageUsages, drawImageExtent);
			aspect = VK_IMAGE_ASPECT_COLOR_BIT;
			break;
		}
		case (ColorFormat::RGBA_FLOAT8): {
			imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
			imageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			imageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			imageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
			imageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			imageUsages |= VK_IMAGE_USAGE_SAMPLED_BIT;
			rimg_info = vkutil::image_create_info(imageFormat, imageUsages, drawImageExtent);
			aspect = VK_IMAGE_ASPECT_COLOR_BIT;
			break;
		}
	default:
		RL_LOG_ERROR("color format not supported yet");
	}

	if(mips != 0) {
		rimg_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(imageExtent.width, imageExtent.height)))) + 1;
	}

	rimg_info.samples = samples;
	VK_CHECK(vmaCreateImage(renderer->allocator, &rimg_info, &rimg_allocinfo, &image, &allocation, nullptr));
	current_layout = VK_IMAGE_LAYOUT_UNDEFINED;

	if(data != nullptr) {
		make_image_from_data(data, imageUsages, mips != 0);
	}
	
	VkImageViewCreateInfo rview_info = vkutil::imageview_create_info(imageFormat, image, aspect);
	VK_CHECK(vkCreateImageView(renderer->device, &rview_info, nullptr, &imageView));

	expose_to_imgui();
}

void VulkanResourceImage::expose_to_imgui() {
	VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;

	switch (iformat) {
	case(ImageFormat::RENDER_ATTACHMENT_COLOR):
		layout = VK_IMAGE_LAYOUT_GENERAL;
		break;

	case(ImageFormat::RENDER_ATTACHMENT_DEPTH):
		layout = VK_IMAGE_LAYOUT_GENERAL;
		break;

	default:
		RL_LOG_WARN("cant deduce image format for imgui");
	}

	imgui_descriptorset = ImGui_ImplVulkan_AddTexture(imageView, layout);
}

void VulkanResourceImage::make_image_from_data(
			void *data, VkImageUsageFlags usage, bool mipmapped) {

	RL_INSTRUMENT_FUNCTION
	RL_LOG_TRACE("making image from data");

	size_t data_size = imageExtent.depth * imageExtent.width * imageExtent.height * 4;
	vkutil::AllocatedBuffer uploadbuffer = vkutil::create_buffer(renderer, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	memcpy(uploadbuffer.info.pMappedData, data, data_size);

	vkutil::immediate_submit(renderer->imm_fence, renderer->imm_command_buffer, renderer->device, renderer->graphics_queue,
	[&](VkCommandBuffer cmd) {
		vkutil::transition_image(cmd, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = imageExtent;

		// copy the buffer into the image
		vkCmdCopyBufferToImage(cmd, uploadbuffer.buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
			&copyRegion);

		vkutil::transition_image(cmd, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		});

	vkutil::destroy_buffer(renderer, uploadbuffer);
	current_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

VulkanResourceImage::~VulkanResourceImage() {
	RL_INSTRUMENT_FUNCTION
	vkDeviceWaitIdle(renderer->device);
	vkDestroyImageView(renderer->device, imageView, nullptr);
	vmaDestroyImage(renderer->allocator, image, allocation);
}

ImTextureID VulkanResourceImage::get_imgui_textureID() {
	transition_image(VK_IMAGE_LAYOUT_GENERAL);
	return (ImTextureID)imgui_descriptorset;
}

void VulkanResourceImage::transition_image(VkImageLayout to_layout) {
	if(current_layout == to_layout) {
		return;
	}

	vkutil::transition_image(
		renderer->get_current_frame().main_command_buffer,
		image, current_layout, to_layout);

	current_layout = to_layout;
}

ImageHandle VulkanResourceImage::get_handle() {
	return imageView;
}

}
