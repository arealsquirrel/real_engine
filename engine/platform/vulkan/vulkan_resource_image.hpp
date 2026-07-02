#ifndef REALLIB_VULKAN_RESOURCE_IMAGE_HPP
#define REALLIB_VULKAN_RESOURCE_IMAGE_HPP

#include "real/core/game.hpp"
#include "real/core/object.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_image.hpp"
#include "vulkan_backend.hpp"
#include <utility>
#include <vulkan/vulkan_core.h>
#include <imgui.h>

namespace real {

class VulkanRenderer;

class VulkanResourceImage : public ResourceImage {
RL_OBJECT(VulkanResourceImage, ResourceImage)

public:
    VulkanResourceImage(
        Instance *_instance,
        u32 width, u32 height,
        ColorFormat cformat, ImageFormat iformat,
		void *data=nullptr, int mips=0, VkSampleCountFlagBits samples=VK_SAMPLE_COUNT_1_BIT,
		std::map<StringHash, Tile> tiles={});

    ~VulkanResourceImage();

	ImageHandle get_handle() override;
	ImTextureID get_imgui_textureID() override;

	void transition_image(VkImageLayout to_layout);

private:
	void make_image_from_data(
			void* data, VkImageUsageFlags usage, bool mipmapped);

	void expose_to_imgui();

public:
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
	VulkanRenderer *renderer;

	VkImageLayout current_layout;

	VkDescriptorSet imgui_descriptorset;
	VkSampleCountFlagBits samples;
};

}

#endif
