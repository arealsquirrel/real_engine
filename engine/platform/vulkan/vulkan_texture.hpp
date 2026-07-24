#ifndef REALLIB_VULKAN_RESOURCE_IMAGE_HPP
#define REALLIB_VULKAN_RESOURCE_IMAGE_HPP

#include "real/core/game.hpp"
#include "real/core/object.hpp"
#include "real/graphics/texture.hpp"
#include "vulkan_backend.hpp"
#include <vulkan/vulkan_core.h>
#include <imgui.h>

namespace real {

class VulkanRenderer;

class VulkanTexture : public Texture {
RL_OBJECT(VulkanTexture, Texture)

public:
    VulkanTexture(
        Instance *_instance,
        u32 width, u32 height,
        ColorFormat cformat, ImageFormat iformat,
		int mips=0, VkSampleCountFlagBits samples=VK_SAMPLE_COUNT_1_BIT);

    ~VulkanTexture();

	void *get_imgui_textureID() override;
	void upload_data(void *data, size_t size) override;

	void transition_image(VkImageLayout to_layout);

private:
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
