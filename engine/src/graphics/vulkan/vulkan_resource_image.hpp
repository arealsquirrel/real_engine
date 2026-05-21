#ifndef REALLIB_VUKLAN_RESOURCE_SHADER_HPP
#define REALLIB_VUKLAN_RESOURCE_SHADER_HPP

#include "real/core/instance.hpp"
#include "real/resource/resource_image.hpp"
#include "vulkan_backend.hpp"
#include <vulkan/vulkan_core.h>

namespace real {

class VulkanRenderer;

class VulkanResourceImage : public ResourceImage {
public:
    VulkanResourceImage(
        Instance *_instance,
        u32 width, u32 height,
        ColorFormat format, void *data=nullptr);

    ~VulkanResourceImage();

	ImageHandle get_handle() override;

public:
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;

	VulkanRenderer *renderer;
};

}

#endif
