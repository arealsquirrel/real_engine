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
		void *data=nullptr,
		std::optional<Path> _path=std::nullopt);

	VulkanResourceImage(
		Instance *_instance,
		VkImage _image, VkImageView _view,
		VkExtent3D _extent, VkFormat _format,
		bool _internaly_managed=false);

    ~VulkanResourceImage();

	ImageHandle get_handle() override;
	ColorFormat get_color_format() override { return ColorFormat::UNKNOWN; }
	std::pair<u32, u32> get_image_extent() override { return std::make_pair(imageExtent.width, imageExtent.height); }
	void transition_image(FrameContext context, ImageFormat to) override;
	ImageFormat get_image_format() override { return iformat; };

	ImTextureID get_imgui_textureID() override;

public:
	const bool internaly_managed;
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
	// ImageFormat engineFormat;
	VulkanRenderer *renderer;

	VkDescriptorSet imgui_descriptorset;
};

}

#endif
