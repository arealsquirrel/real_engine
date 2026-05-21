#ifndef REALLIB_VULKAN_RENDERER_HPP
#define REALLIB_VULKAN_RENDERER_HPP

#include "real/core/instance.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "vulkan_backend.hpp"
#include "vulkan_descriptor_allocator.hpp"
#include "vulkan_util.hpp"
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>

namespace real {

constexpr int VULKAN_FRAME_OVERLAP = 2;

class VulkanResourceImage;

struct FrameDataVulkan {
    VkCommandPool command_pool;
    VkCommandBuffer main_command_buffer;
    VkSemaphore swapchain_semaphores;
	VkFence render_fence;
    VkExtent2D draw_extent;
    u32 swapchain_index;
    vkutil::DeletionQueue delete_queue;
};

class VulkanRenderer : public Renderer {
public:
	VulkanRenderer(Instance *_instance, Shared<Window> _window);
	~VulkanRenderer();

	FrameContext start_frame() override;
	void end_frame(FrameContext context) override;

private:
	void create_imgui();
	void create_descriptors();
	void create_frame_objects(); // also sync data for swapchain images
	void create_swapchain(u32 width, u32 height);
	void create_device();
	void create_queues();
	void create_vma();

	void destroy_swapchain();

public:
	VkDevice device;
    DescriptorAllocator descriptor_allocator;
    VmaAllocator allocator;

private:
	vkb::Device vkbDevice;
    VkPhysicalDevice chosenGPU;
	VkSurfaceKHR surface;
    VkQueue graphics_queue;
    uint32_t graphics_queue_family;

    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_views;
    std::vector<VkSemaphore> render_semaphore;
    VkExtent2D swapchain_extent;
    VkSwapchainKHR swapchain;
	VkFormat swapchain_image_format;

    FrameDataVulkan frame_data[VULKAN_FRAME_OVERLAP];
    vkutil::DeletionQueue delete_queue;
    // VulkanResourceImage *render_image;
    VkFence imm_fence;
    VkCommandBuffer imm_command_buffer;
    VkCommandPool imm_command_pool;
    VkDescriptorPool imgui_descriptor_pool;
    u32 frame_number=0;
};

}

#endif
