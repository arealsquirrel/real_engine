#ifndef REALLIB_VULKAN_BACKEND_HPP
#define REALLIB_VULKAN_BACKEND_HPP

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <VkBootstrap.h>

#include "vk_mem_alloc.h"

namespace real {

// constexpr int VULKAN_FRAME_OVERLAP = 2;

/**
 * @brief this is the backend per application.
 */

struct GraphicsBackendVulkan {
    vkb::Instance vkbInstance;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
};

/*
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
*/

struct ImageVulkan {
    
};

/*
struct RendererDataVulkan {
    FrameDataVulkan frame_data[VULKAN_FRAME_OVERLAP];
    vkutil::DeletionQueue delete_queue;
    ResourceImage *render_image;
    VkFence imm_fence;
    VkCommandBuffer imm_command_buffer;
    VkCommandPool imm_command_pool;
    VkDescriptorPool imgui_descriptor_pool;
};
*/

struct RenderPassVulkan {
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkDescriptorSet descriptor_set;
    VkDescriptorSetLayout descriptor_set_layout;
};

}

#endif
