#ifndef REALLIB_VULKAN_BACKEND_HPP
#define REALLIB_VULKAN_BACKEND_HPP

#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include "real/core/types.hpp"
#include "real/resource/resource_image.hpp"
#include "vulkan_descriptor_allocator.hpp"
#include "vulkan_util.hpp"
#include <VkBootstrap.h>

#include "vk_mem_alloc.h"

namespace real {

constexpr int VULKAN_FRAME_OVERLAP = 2;

/**
 * @brief this is the backend per application.
 */
struct GraphicsBackendVulkan {
    vkb::Instance vkbInstance;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
};

/**
 * @brief this is the backend per window
 */
struct WindowBackendVulkan {
    VkPhysicalDevice chosenGPU;
	VkDevice device;
	VkSurfaceKHR surface;
    VkQueue graphics_queue;
    uint32_t graphics_queue_family;

    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_views;
    std::vector<VkSemaphore> render_semaphore;
    VkExtent2D swapchain_extent;
    VkSwapchainKHR swapchain;
	VkFormat swapchain_image_format;

    DescriptorAllocator descriptor_allocator;
    VmaAllocator allocator;
};

/**
 * @brief per frame data for the swapchain :((
 * 
 */
struct FrameDataVulkan {
    VkCommandPool command_pool;
    VkCommandBuffer main_command_buffer;
    VkSemaphore swapchain_semaphores;
	VkFence render_fence;
    VkExtent2D draw_extent;
    u32 swapchain_index;
    vkutil::DeletionQueue delete_queue;
};

struct ImageVulkan {
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
};

struct RendererDataVulkan {
    FrameDataVulkan frame_data[VULKAN_FRAME_OVERLAP];
    vkutil::DeletionQueue delete_queue;
    ResourceImage *render_image;
};

struct ShaderVulkan {
    VkShaderModule module;
    // std::vector<VkDescriptorSetLayoutBinding> bindings;
};

struct RenderPassVulkan {
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkDescriptorSet descriptor_set;
    VkDescriptorSetLayout descriptor_set_layout;
};


}

#endif