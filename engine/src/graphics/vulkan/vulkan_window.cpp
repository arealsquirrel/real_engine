
#include "vulkan_util.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <VkBootstrap.h>
#include "vulkan_backend.hpp"
#include <vulkan/vulkan_core.h>

#include <real/graphics/window.hpp>
#include <real/core/instance.hpp>
#include "real/graphics/graphics.hpp"
#include "vulkan_util.hpp"
#include "vulkan_backend.hpp"

#include <cassert>

namespace real {

u32 Window::s_window_count = 0;

Window::Window(Instance *_instance, const WindowInfo &info) 
    : instance(_instance) {

    if(s_window_count++ == 0) {
        instance->log.info("initilizing GLFW {}:{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
        glfwSetErrorCallback(real_glfw_error_callback);
        assert(glfwInit());
    }

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(info.width, info.height, info.title, NULL, NULL);

    /* now we can set up the swapchain and whatever for this window */
    WindowBackendVulkan *window_backend = new WindowBackendVulkan;
    GraphicsBackendVulkan *vulkan_backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    glfwCreateWindowSurface(vulkan_backend->instance, window, NULL, &window_backend->surface);   
    
    VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features.dynamicRendering = true;
	features.synchronization2 = true;

	VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress = true;
	features12.descriptorIndexing = true;

	vkb::PhysicalDeviceSelector selector { vulkan_backend->vkbInstance };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 3)
		.set_required_features_13(features)
		.set_required_features_12(features12)
		.set_surface(window_backend->surface)
		.select()
		.value();

    instance->log.info("vulkan rendering device found: {}", physicalDevice.name);

    vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	vkb::Device vkbDevice = deviceBuilder.build().value();

	window_backend->device = vkbDevice.device;
	window_backend->chosenGPU = physicalDevice.physical_device;

    /* ----------- CREATE SWAPCHAIN ----------- */
    vkb::SwapchainBuilder swapchainBuilder{ window_backend->chosenGPU, window_backend->device, window_backend->surface };

	window_backend->swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		//.use_default_format_selection()
		.set_desired_format(VkSurfaceFormatKHR{ .format = window_backend->swapchain_image_format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		//use vsync present mode
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(info.width, info.height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build()
		.value();

	window_backend->swapchain_extent = vkbSwapchain.extent;
	window_backend->swapchain = vkbSwapchain.swapchain;
	window_backend->swapchain_images = vkbSwapchain.get_images().value();
	window_backend->swapchain_views = vkbSwapchain.get_image_views().value();

    window_backend->graphics_queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	window_backend->graphics_queue_family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();
	for (int i = 0; i < window_backend->swapchain_images.size(); i++) {
		VkSemaphore s;
		VK_CHECK(vkCreateSemaphore(
            window_backend->device,
            &semaphoreCreateInfo,
            nullptr, &s));
		window_backend->render_semaphore.push_back(s);
	}

    backend = window_backend;
}

Window::~Window() {
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)backend;
    GraphicsBackendVulkan *vulkan_backend = (GraphicsBackendVulkan*)Graphics::get_backend();

    vkDestroySwapchainKHR(window_backend->device, window_backend->swapchain, nullptr);

	for (int i = 0; i < window_backend->swapchain_views.size(); i++) {
		vkDestroySemaphore(window_backend->device, window_backend->render_semaphore[i], nullptr);
		vkDestroyImageView(window_backend->device, window_backend->swapchain_views[i], nullptr);
	}
    
    vkDestroySurfaceKHR(vulkan_backend->instance, window_backend->surface, nullptr);
    vkDestroyDevice(window_backend->device, nullptr);

    glfwDestroyWindow(window);

    if(--s_window_count == 0) {
        instance->log.info("killing GLFW");
        glfwTerminate();
    }
}

WindowBackend Window::backend_handle() {
    return backend;
}

bool Window::should_exit() {
    return glfwWindowShouldClose(window);
}

void Window::swap_buffers() {
    return glfwSwapBuffers(window);
}

}
