
#include "imgui_impl_vulkan.h"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include "vulkan_backend.hpp"
#include <real/resource/resource_image.hpp>
#include <tracy/Tracy.hpp>
#include <vulkan/vulkan_core.h>
#include "vulkan_texture.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_util.hpp"
#include "vulkan_renderer.hpp"

namespace real {

VulkanTexture::VulkanTexture(
    Instance *_instance,
    u32 width, u32 height, 
	ColorFormat _cformat, ImageFormat _iformat,
	int mips, VkSampleCountFlagBits _samples)
    : Texture(_instance, width, height, _cformat, _iformat),
	  samples(_samples) {
	
	ZoneScoped
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
			imageFormat = VK_FORMAT_D32_SFLOAT;
			imageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageUsages |= VK_IMAGE_USAGE_SAMPLED_BIT;
			rimg_info = vkutil::image_create_info(imageFormat, imageUsages, drawImageExtent);
			aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
			break;
		}
	
		case (ColorFormat::RGBA_FLOAT16): {
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

	// if(data != nullptr) {
	// 	make_image_from_data(data, imageUsages, mips != 0);
	// }
	
	VkImageViewCreateInfo rview_info = vkutil::imageview_create_info(imageFormat, image, aspect);
	VK_CHECK(vkCreateImageView(renderer->device, &rview_info, nullptr, &imageView));

	expose_to_imgui();
}

void VulkanTexture::expose_to_imgui() {
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

void VulkanTexture::upload_data(void *data, size_t size) {
	ZoneScoped

	size_t data_size = imageExtent.depth * imageExtent.width * imageExtent.height * 4;
	vkutil::AllocatedBuffer uploadbuffer = vkutil::create_buffer(renderer, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	memcpy(uploadbuffer.info.pMappedData, data, size);

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

VulkanTexture::~VulkanTexture() {
	ZoneScoped
	vkDeviceWaitIdle(renderer->device);
	vkDestroyImageView(renderer->device, imageView, nullptr);
	vmaDestroyImage(renderer->allocator, image, allocation);
}

void *VulkanTexture::get_imgui_textureID() {
	transition_image(VK_IMAGE_LAYOUT_GENERAL);
	return (void*)imgui_descriptorset;
}

void VulkanTexture::transition_image(VkImageLayout to_layout) {
	ZoneScoped

	if(current_layout == to_layout) {
		return;
	}

	vkutil::transition_image(
		renderer->get_current_frame().main_command_buffer,
		image, current_layout, to_layout);

	current_layout = to_layout;
}

UniquePointer<Texture> Texture::create(
			Instance *instance, u32 width, u32 height,
			ColorFormat cformat, ImageFormat iformat, int mips) {


    return UniquePointer<Texture>(
			&instance->engine_allocator,
			(Texture*)instance->engine_allocator.allocate_object<VulkanTexture>(instance, width, height, cformat, iformat, mips, VK_SAMPLE_COUNT_1_BIT));
}


}
