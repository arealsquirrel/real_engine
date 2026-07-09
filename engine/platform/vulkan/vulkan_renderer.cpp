
#include "real/core/event.hpp"
#include "real/core/event_listener.hpp"
#include "real/core/game.hpp"
#include "real/core/logging.hpp"

#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/graphics/imgui_style.hpp"
#include "vulkan_resource_image.hpp"
#include "real/resource/resource_image.hpp"

#include "real/graphics/graphics.hpp"
#include "vulkan_backend.hpp"
#include <cassert>
#include "vulkan_resource_image.hpp"
#include <real/graphics/renderer.hpp>
#include <vulkan/vulkan_core.h>
#include "vulkan_util.hpp"
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include "vulkan_renderer.hpp"
#include "vulkan_resource_image.hpp"

#pragma clang diagnostic push             // Save current state
#pragma clang diagnostic ignored "-Wnullability-completeness" // Disable a specific flag (e.g., -Wunused-variable)
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#pragma clang diagnostic pop

#include <real/real.hpp>

#include "real/resource/resource_image.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace real {

VulkanRenderer::VulkanRenderer(Instance *_instance, Shared<Window> _window)
    : Renderer(_instance, _window), EventListener(_instance, this) {}

void VulkanRenderer::init() {
    RL_INSTRUMENT_FUNCTION

	RL_LOG_TRACE("Creating vulkan renderer");
	auto [width, height] = window->get_glfw_window_dimensions();

	create_device();
	create_swapchain(width, height);
	create_queues();
	create_vma();
	create_frame_objects();
	create_descriptors();
	create_imgui();

	VkSamplerCreateInfo sampl = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};

	sampl.magFilter = VK_FILTER_NEAREST;
	sampl.minFilter = VK_FILTER_NEAREST;
	vkCreateSampler(device, &sampl, nullptr, &samplerNearest);

	sampl.magFilter = VK_FILTER_LINEAR;
	sampl.minFilter = VK_FILTER_LINEAR;
	vkCreateSampler(device, &sampl, nullptr, &samplerLinear);

	event_subscribe<EventWindowResize>(
		[&](EventWindowResize &resize, Object *object){
		should_resize = true;
	});

	scene_data = UniformBuffer::create(instance, sizeof(SceneData));
}

VulkanRenderer::~VulkanRenderer() {
    RL_INSTRUMENT_FUNCTION

    GraphicsBackendVulkan *vulkan_backend = (GraphicsBackendVulkan*)Graphics::get_backend();
	RL_LOG_TRACE("destroying vulkan renderer");
    vkDeviceWaitIdle(device);

	delete scene_data.release();

	RL_LOG_INFO("rawr");

    ImGui_ImplVulkan_Shutdown();
	vkDestroyDescriptorPool(device, imgui_descriptor_pool, nullptr);
	vkDestroySampler(device, samplerLinear, nullptr);
	vkDestroySampler(device, samplerNearest, nullptr);

    delete_queue.flush();

	for (int i = 0; i < swapchain_views.size(); i++) {
		vkDestroySemaphore(device, render_semaphore[i], nullptr);
	}


    for (int i = 0; i < VULKAN_FRAME_OVERLAP; i++) {
		frame_data[i].frameDescriptors.destroy_pools(device);

        vkDestroyCommandPool(device, frame_data[i].command_pool, nullptr);

	    vkDestroyFence(device, frame_data[i].render_fence, nullptr);
	    vkDestroySemaphore(device, frame_data[i].swapchain_semaphores, nullptr);
    
        frame_data[i].delete_queue.flush();
    }

	vkDestroyFence(device, imm_fence, nullptr);
    vkDestroyCommandPool(device, imm_command_pool, nullptr);

	destroy_swapchain();
	vkDestroySurfaceKHR(vulkan_backend->instance, surface, nullptr);

	descriptor_allocator.destroy_pool(device);

	vmaDestroyAllocator(allocator);

	vkDestroyDevice(device, nullptr);
}

void VulkanRenderer::create_imgui() {
    RL_INSTRUMENT_FUNCTION

	RL_LOG_TRACE("Vulkan renderer creating imgui");
    GraphicsBackendVulkan *vulkan_backend = (GraphicsBackendVulkan*)Graphics::get_backend();

    VkDescriptorPoolSize pool_sizes[] = 
      { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
	   { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VK_CHECK(vkCreateDescriptorPool(device, &pool_info, nullptr, &imgui_descriptor_pool));

	ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    ImGui_ImplGlfw_InitForVulkan(window->glfw_window(), true);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = vulkan_backend->instance;
	init_info.PhysicalDevice = chosenGPU;
	init_info.Device = device;
	init_info.Queue = graphics_queue;
	init_info.DescriptorPool = imgui_descriptor_pool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.UseDynamicRendering = true;

    ImGui_ImplVulkan_PipelineInfo pipe_info = {};
	pipe_info.PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
	pipe_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	pipe_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchain_image_format;
	pipe_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.PipelineInfoMain = pipe_info;

	ImGui_ImplVulkan_Init(&init_info);

    RealImguiStyle::style_imgui();
}

void VulkanRenderer::create_descriptors() {
    RL_INSTRUMENT_FUNCTION

	RL_LOG_TRACE("Vulkan renderer creating descriptors");
	descriptor_allocator.init_pool(device, 10,{
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 } 
	});
}

void VulkanRenderer::create_vma() {
    RL_INSTRUMENT_FUNCTION
	RL_LOG_TRACE("Vulkan renderer creating vma");
    GraphicsBackendVulkan *vulkan_backend = (GraphicsBackendVulkan*)Graphics::get_backend();
	VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = chosenGPU;
    allocatorInfo.device = device;
    allocatorInfo.instance = vulkan_backend->instance;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    vmaCreateAllocator(&allocatorInfo, &allocator);
}

void VulkanRenderer::create_queues() {
    RL_INSTRUMENT_FUNCTION
	RL_LOG_TRACE("Vulkan renderer creating queues");
    graphics_queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	graphics_queue_family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}

void VulkanRenderer::create_frame_objects() {
    RL_INSTRUMENT_FUNCTION
	RL_LOG_TRACE("Vulkan renderer creating frame objects");
    VkFenceCreateInfo fenceCreateInfo = vkutil::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkutil::semaphore_create_info();
    VkCommandPoolCreateInfo commandPoolInfo = vkutil::command_pool_create_info(graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &imm_command_pool));
	VkCommandBufferAllocateInfo cmdAllocInfo = vkutil::command_buffer_allocate_info(imm_command_pool, 1);
	VK_CHECK(vkAllocateCommandBuffers(device, &cmdAllocInfo, &imm_command_buffer));

	for (int i = 0; i < swapchain_images.size(); i++) {
		VkSemaphore s;
		VK_CHECK(vkCreateSemaphore(
            device,
            &semaphoreCreateInfo,
            nullptr, &s));
		render_semaphore.push_back(s);
	}

	for (int i = 0; i < VULKAN_FRAME_OVERLAP; i++) {

		std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frame_sizes = { 
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
		};

		frame_data[i].frameDescriptors = DescriptorAllocatorGrowable{};
		frame_data[i].frameDescriptors.init(device, 1000, frame_sizes);
	
        /* COMMAND POOLS AND BUFFERS */
		VK_CHECK(vkCreateCommandPool(
            device,
            &commandPoolInfo,
            nullptr, &frame_data[i].command_pool));
		VkCommandBufferAllocateInfo cmdAllocInfo = vkutil::command_buffer_allocate_info(
            frame_data[i].command_pool, 1);

		VK_CHECK(vkAllocateCommandBuffers(
            device, &cmdAllocInfo,
             &frame_data[i].main_command_buffer));
	

        /* SYNC OBJECTS */
        VK_CHECK(vkCreateFence(
            device,
            &fenceCreateInfo, nullptr,
            &frame_data[i].render_fence));
	
		VK_CHECK(vkCreateSemaphore(
            device,
            &semaphoreCreateInfo,
            nullptr, &frame_data[i].swapchain_semaphores));
    }

    VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &imm_fence));
}

void VulkanRenderer::create_swapchain(u32 width, u32 height) {
    RL_INSTRUMENT_FUNCTION
	RL_LOG_TRACE("Vulkan renderer creating swapchain");
    vkb::SwapchainBuilder swapchainBuilder{ chosenGPU, device, surface };

	swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.set_desired_format(VkSurfaceFormatKHR{ .format = swapchain_image_format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(width, height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build()
		.value();

	swapchain_extent = vkbSwapchain.extent;
	swapchain = vkbSwapchain.swapchain;
	swapchain_images = vkbSwapchain.get_images().value();
	swapchain_views = vkbSwapchain.get_image_views().value();
}

void VulkanRenderer::destroy_swapchain() {
    RL_INSTRUMENT_FUNCTION
    GraphicsBackendVulkan *vulkan_backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    vkDestroySwapchainKHR(device, swapchain, nullptr);

	for (int i = 0; i < swapchain_views.size(); i++) {
		vkDestroyImageView(device, swapchain_views[i], nullptr);
	}
}

void VulkanRenderer::create_device() {
    RL_INSTRUMENT_FUNCTION
	RL_LOG_TRACE("Vulkan renderer creating device");
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();

    GraphicsBackendVulkan *vulkan_backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    glfwCreateWindowSurface(vulkan_backend->instance, window->glfw_window(), NULL, &surface);   
    
    VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features.dynamicRendering = true;
	features.synchronization2 = true;

	VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress = true;
	features12.descriptorIndexing = true;

	VkPhysicalDeviceVulkan11Features features11 = {};
	features11.shaderDrawParameters = true;

	vkb::PhysicalDeviceSelector selector { vulkan_backend->vkbInstance };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 3)
		.set_required_features_13(features)
		.set_required_features_12(features12)
		.set_required_features_11(features11)
		.set_surface(surface)
		.select()
		.value();

    RL_LOG_INFO("vulkan rendering device found: {}", physicalDevice.name);
	VkSampleCountFlags counts = physicalDevice.properties.limits.framebufferDepthSampleCounts & physicalDevice.properties.limits.framebufferColorSampleCounts;
    if (counts & VK_SAMPLE_COUNT_2_BIT)  { samples = VK_SAMPLE_COUNT_2_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT)  { samples = VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT)  { samples = VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { samples = VK_SAMPLE_COUNT_16_BIT; }
    
	RL_LOG_INFO("Picked multisampling {}", counts);
	
    vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	vkbDevice = deviceBuilder.build().value();
	device = vkbDevice.device;
	chosenGPU = physicalDevice.physical_device;
}

void VulkanRenderer::swapchain_resize() {
    RL_LOG_TRACE("Resizing window");
    should_resize = false;

    auto [width, height] = window->get_glfw_window_dimensions();
	
	vkDeviceWaitIdle(device);
    destroy_swapchain();
    create_swapchain(width, height);
}

void VulkanRenderer::start_frame() {
    RL_INSTRUMENT_FUNCTION
    if(should_resize)
        swapchain_resize();

    render_stats.draw_calls = 0;
    render_stats.verticies = 0;
    render_stats.indicies = 0;
    render_stats.frame_time.restart();

    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    FrameDataVulkan *frame = &frame_data[frame_number % VULKAN_FRAME_OVERLAP];

    VK_CHECK(vkWaitForFences(
        device, 1,
        &frame->render_fence, true, 1000000000));
	VK_CHECK(vkResetFences(
        device, 1,
        &frame->render_fence));
	VkResult res = vkAcquireNextImageKHR(
        device,
        swapchain,
        1000000000, frame->swapchain_semaphores,
        nullptr, &frame->swapchain_index);

    if(res == VK_ERROR_OUT_OF_DATE_KHR) {
        RL_LOG_INFO("out of date in start");
        swapchain_resize();
        start_frame();
        return;
    }

    VkCommandBuffer cmd = frame->main_command_buffer;
	VK_CHECK(vkResetCommandBuffer(cmd, 0));
	VkCommandBufferBeginInfo cmdBeginInfo = vkutil::command_buffer_begin_info(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

	transition_swapchain(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
}

void VulkanRenderer::end_frame(const ResourceImage *copy_to_screen_image) {
    RL_INSTRUMENT_FUNCTION
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    FrameDataVulkan &frame = get_current_frame();
    VkCommandBuffer cmd = frame.main_command_buffer;

	if(copy_to_screen_image != nullptr) {
		VulkanResourceImage *vk_resolve_image = (VulkanResourceImage*)copy_to_screen_image;
		vk_resolve_image->transition_image(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		VkExtent2D draw_extent;
		draw_extent.width = vk_resolve_image->get_image_extent().first;
		draw_extent.height = vk_resolve_image->get_image_extent().second;
		
		vkutil::copy_image_to_image(
				get_current_frame().main_command_buffer,
				vk_resolve_image->image, swapchain_images[get_current_frame().swapchain_index],
			draw_extent, swapchain_extent);
	}

	ImGui::Render();

	transition_swapchain(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkRenderingAttachmentInfo colorAttachment = vkutil::attachment_info(swapchain_views[frame.swapchain_index], nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkRenderingInfo renderInfo = vkutil::rendering_info(swapchain_extent, &colorAttachment, nullptr);

	vkCmdBeginRendering(cmd, &renderInfo);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	vkCmdEndRendering(cmd);

	transition_swapchain(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    VK_CHECK(vkEndCommandBuffer(cmd));
    
    VkCommandBufferSubmitInfo cmdinfo = vkutil::command_buffer_submit_info(cmd);	
    VkSemaphoreSubmitInfo waitInfo = vkutil::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        frame.swapchain_semaphores);
    VkSemaphoreSubmitInfo signalInfo = vkutil::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            	render_semaphore[frame.swapchain_index]);	
    VkSubmitInfo2 submit = vkutil::submit_info(
        &cmdinfo,&signalInfo
        ,&waitInfo);	
    VK_CHECK(vkQueueSubmit2(
        graphics_queue, 1,
        &submit, frame.render_fence));

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pWaitSemaphores = &render_semaphore[frame.swapchain_index];
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &frame.swapchain_index;

    VkResult res = vkQueuePresentKHR(graphics_queue, &presentInfo);
    if(res == VK_ERROR_OUT_OF_DATE_KHR) {
        RL_LOG_INFO("out of date");
        should_resize = true;
    }

    frame.delete_queue.flush();
	get_current_frame().current_swapchain_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    frame_number++;
    render_stats.frame_time.stop();
}

void VulkanRenderer::transition_swapchain(VkImageLayout to_layout) {
	VkImageLayout current_swapchain_layout = get_current_frame().current_swapchain_layout;

	if(current_swapchain_layout == to_layout) {
		return;
	}

	vkutil::transition_image(
		get_current_frame().main_command_buffer,
		swapchain_images[get_current_frame().swapchain_index],
		current_swapchain_layout, to_layout);

	get_current_frame().current_swapchain_layout = to_layout;
}

FrameDataVulkan &VulkanRenderer::get_current_frame() {
    return frame_data[frame_number % VULKAN_FRAME_OVERLAP];
}

Unique<Renderer> Renderer::create(Instance *instance, Shared<Window> window) {
	return std::make_unique<VulkanRenderer>(instance, window);
}

}
