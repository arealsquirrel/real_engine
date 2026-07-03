#ifndef REALLIB_VULKAN_RENDERER_HPP
#define REALLIB_VULKAN_RENDERER_HPP

#include "real/core/event.hpp"
#include "real/core/object.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include "vulkan_backend.hpp"
#include "vulkan_descriptor_allocator.hpp"
#include "vulkan_util.hpp"
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>
#include <real/core/event_listener.hpp>

namespace real {

constexpr int VULKAN_FRAME_OVERLAP = 2;

struct FrameDataVulkan {
    VkCommandPool command_pool;
    VkCommandBuffer main_command_buffer;
    VkSemaphore swapchain_semaphores;
	VkFence render_fence;
    u32 swapchain_index;
    vkutil::DeletionQueue delete_queue;
	DescriptorAllocatorGrowable frameDescriptors;
	VkImageLayout current_swapchain_layout {VK_IMAGE_LAYOUT_UNDEFINED};
};

class VulkanRenderer : public Renderer, EventListener {
RL_OBJECT(VulkanRenderer, Renderer)

public:
	VulkanRenderer(Instance *_instance, Shared<Window> _window);
	~VulkanRenderer();

	void start_frame() final override;
	void end_frame(const ResourceImage *copy_to_screen_image) final override;
	void init() final override;

    FrameDataVulkan &get_current_frame();

	void transition_swapchain(VkImageLayout to);

private:
	void create_imgui();
	void create_descriptors();
	void create_frame_objects(); // also sync data for swapchain images
	void create_swapchain(u32 width, u32 height);
	void create_device();
	void create_queues();
	void create_vma();

	void destroy_swapchain();

    void swapchain_resize();

public:
	VkDevice device;
    DescriptorAllocator descriptor_allocator;
    VmaAllocator allocator;
    VkFence imm_fence;
    VkCommandBuffer imm_command_buffer;
    VkCommandPool imm_command_pool;
    VkQueue graphics_queue;

	VkSampler samplerNearest;
	VkSampler samplerLinear;

	VkSampleCountFlagBits samples;
	
private:
	vkb::Device vkbDevice;
    VkPhysicalDevice chosenGPU;
	VkSurfaceKHR surface;
    uint32_t graphics_queue_family;

    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_views;
    std::vector<VkSemaphore> render_semaphore;
    VkExtent2D swapchain_extent;
    VkSwapchainKHR swapchain;
	VkFormat swapchain_image_format;

    FrameDataVulkan frame_data[VULKAN_FRAME_OVERLAP];
    vkutil::DeletionQueue delete_queue;
    VkDescriptorPool imgui_descriptor_pool;
    u32 frame_number=0;
    bool should_resize {false};
};

}

#endif
