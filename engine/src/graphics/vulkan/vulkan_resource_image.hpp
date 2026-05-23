#ifndef REALLIB_VUKLAN_RESOURCE_SHADER_HPP
#define REALLIB_VUKLAN_RESOURCE_SHADER_HPP

#include "real/core/instance.hpp"
#include "real/resource/resource_image.hpp"
#include "vulkan_backend.hpp"
#include <utility>
#include <vulkan/vulkan_core.h>

namespace real {

class VulkanRenderer;

class VulkanResourceImage : public ResourceImage {
public:
    VulkanResourceImage(
        Instance *_instance,
        u32 width, u32 height,
        ColorFormat format, void *data=nullptr);

	VulkanResourceImage(Instance *_instance,
			VkImage _image, VkImageView _view,
			VkExtent3D _extent, VkFormat _format,
			bool _internaly_managed=false);

    ~VulkanResourceImage();

	ImageHandle get_handle() override;
	ColorFormat get_format() override { return ColorFormat::UNKNOWN; }
	std::pair<u32, u32> get_image_extent() override { return std::make_pair(imageExtent.width, imageExtent.height); }

public:
	const bool internaly_managed;
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;

	VulkanRenderer *renderer;
};

}

#endif
