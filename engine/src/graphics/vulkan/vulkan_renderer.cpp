
#include "real/core/game.hpp"
#include "real/core/logging.hpp"

#include "real/core/types.hpp"
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

void style_imgui();

VulkanRenderer::VulkanRenderer(Instance *_instance, Shared<Window> _window)
    : Renderer(_instance, _window) {}

void VulkanRenderer::init() {
	RL_LOG_TRACE("Creating vulkan renderer");
	auto [width, height] = window->get_glfw_window_dimensions();

	create_device();
	create_swapchain(width, height);
	create_queues();
	create_vma();
	create_frame_objects();
	create_descriptors();
	create_imgui();

	instance->resource_database->register_resource(
 		ResourceImage::create(instance, width, height,
			ColorFormat::RGBA_FLOAT, ImageFormat::RENDER_ATTACHMENT_COLOR), "_render_color_texture");

	instance->resource_database->register_resource(
			ResourceImage::create(instance, width, height,
				ColorFormat::DEPTH, ImageFormat::RENDER_ATTACHMENT_DEPTH), "_render_depth_texture");

	renderImage = instance->resource_database->get_resource<ResourceImage>("_render_color_texture");
}

VulkanRenderer::~VulkanRenderer() {
	RL_LOG_TRACE("destroying vulkan renderer");
    vkDeviceWaitIdle(device);

    ImGui_ImplVulkan_Shutdown();
	vkDestroyDescriptorPool(device, imgui_descriptor_pool, nullptr);

    delete_queue.flush();

    for (int i = 0; i < VULKAN_FRAME_OVERLAP; i++) {
        vkDestroyCommandPool(device, frame_data[i].command_pool, nullptr);

	    vkDestroyFence(device, frame_data[i].render_fence, nullptr);
	    vkDestroySemaphore(device, frame_data[i].swapchain_semaphores, nullptr);
    
        frame_data[i].delete_queue.flush();
    }

	vkDestroyFence(device, imm_fence, nullptr);
    vkDestroyCommandPool(device, imm_command_pool, nullptr);

	destroy_swapchain();

	descriptor_allocator.destroy_pool(device);

	vmaDestroyAllocator(allocator);

	vkDestroyDevice(device, nullptr);
}

void VulkanRenderer::create_imgui() {
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
    style_imgui();

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
}

void VulkanRenderer::create_descriptors() {
	RL_LOG_TRACE("Vulkan renderer creating descriptors");
	descriptor_allocator.init_pool(device, 10,{
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 } 
	});
}

void VulkanRenderer::create_vma() {
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
	RL_LOG_TRACE("Vulkan renderer creating queues");
    graphics_queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	graphics_queue_family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}

void VulkanRenderer::create_frame_objects() {
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
    GraphicsBackendVulkan *vulkan_backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    vkDestroySwapchainKHR(device, swapchain, nullptr);

	for (int i = 0; i < swapchain_views.size(); i++) {
		vkDestroySemaphore(device, render_semaphore[i], nullptr);
		vkDestroyImageView(device, swapchain_views[i], nullptr);
	}
    
    vkDestroySurfaceKHR(vulkan_backend->instance, surface, nullptr);
}

void VulkanRenderer::create_device() {
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

    vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	vkbDevice = deviceBuilder.build().value();
	device = vkbDevice.device;
	chosenGPU = physicalDevice.physical_device;
}

FrameContext VulkanRenderer::start_frame() {
    // RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 
    // WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    FrameDataVulkan *frame = &frame_data[frame_number % VULKAN_FRAME_OVERLAP];
	VulkanResourceImage *render_image_handle = (VulkanResourceImage*)(renderImage.get());
    frame->draw_extent.width = render_image_handle->imageExtent.width;
    frame->draw_extent.height = render_image_handle->imageExtent.height;

    VK_CHECK(vkWaitForFences(
        device, 1,
        &frame->render_fence, true, 1000000000));
	VK_CHECK(vkResetFences(
        device, 1,
        &frame->render_fence));
	VK_CHECK(vkAcquireNextImageKHR(
        device,
        swapchain,
        1000000000, frame->swapchain_semaphores,
        nullptr, &frame->swapchain_index));

    VkCommandBuffer cmd = frame->main_command_buffer;
	VK_CHECK(vkResetCommandBuffer(cmd, 0));
	VkCommandBufferBeginInfo cmdBeginInfo = vkutil::command_buffer_begin_info(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    // transition our main draw image into general layout so we can write into it
	// we will overwrite it all so we dont care about what was the older layout
	vkutil::transition_image(cmd, render_image_handle->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    return frame;
}

void VulkanRenderer::end_frame(FrameContext context) {
    // RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 
    // WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    FrameDataVulkan *frame = (FrameDataVulkan*)context;
    VkCommandBuffer cmd = frame->main_command_buffer;
	VulkanResourceImage *render_image_handle = (VulkanResourceImage*)(renderImage.get());
 
	ImGui::Render();


	//transition the draw image and the swapchain image into their correct transfer layouts
	vkutil::transition_image(cmd, render_image_handle->image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vkutil::transition_image(cmd, swapchain_images[frame->swapchain_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vkutil::copy_image_to_image(cmd, render_image_handle->image, swapchain_images[frame->swapchain_index], frame->draw_extent, swapchain_extent);
	vkutil::transition_image(cmd, swapchain_images[frame->swapchain_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkRenderingAttachmentInfo colorAttachment = vkutil::attachment_info(swapchain_views[frame->swapchain_index], nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkRenderingInfo renderInfo = vkutil::rendering_info(swapchain_extent, &colorAttachment, nullptr);
	vkCmdBeginRendering(cmd, &renderInfo);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	vkCmdEndRendering(cmd);

    vkutil::transition_image(cmd, swapchain_images[frame->swapchain_index], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    VK_CHECK(vkEndCommandBuffer(cmd));
    
    VkCommandBufferSubmitInfo cmdinfo = vkutil::command_buffer_submit_info(cmd);	
    VkSemaphoreSubmitInfo waitInfo = vkutil::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        frame->swapchain_semaphores);
    VkSemaphoreSubmitInfo signalInfo = vkutil::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            	render_semaphore[frame->swapchain_index]);	
    VkSubmitInfo2 submit = vkutil::submit_info(
        &cmdinfo,&signalInfo
        ,&waitInfo);	
    VK_CHECK(vkQueueSubmit2(
        graphics_queue, 1,
        &submit, frame->render_fence));

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pWaitSemaphores = &render_semaphore[frame->swapchain_index];
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &frame->swapchain_index;

    VK_CHECK(vkQueuePresentKHR(graphics_queue, &presentInfo));
    frame->delete_queue.flush();
    frame_number++;
}

ImVec4 ImGuiIntRGBToFloatRGB(int r, int g, int b){
    return ImVec4(r / 256.0f, g / 256.0f, b / 256.0f, 1.0f);
}

void style_imgui() {      
    ImGuiStyle *style = &ImGui::GetStyle();
    style->WindowMinSize = ImVec2(160, 20);
    style->FramePadding = ImVec2(4, 4);
    style->ItemSpacing = ImVec2(6, 4);
    style->PopupRounding = 6.0f;
    style->ItemInnerSpacing = ImVec2(6, 4);
    style->Alpha = 0.95f;
    style->FrameRounding = 6.0f;
    style->IndentSpacing = 6.0f;
    style->ColumnsMinSpacing = 50.0f;
    style->GrabMinSize = 14.0f;
    style->GrabRounding = 16.0f;
    style->ScrollbarSize = 12.0f;
    style->ScrollbarRounding = 16.0f;
    style->WindowRounding = 8.0f;
    // style->DockingSeparatorSize = 6.0f;

    ImVec4 darkWindowBackground = ImGuiIntRGBToFloatRGB(29, 32, 33);
    ImVec4 windowBackground = ImGuiIntRGBToFloatRGB(40, 40, 40);
    ImVec4 textColor = ImGuiIntRGBToFloatRGB(235, 219, 178);
    ImVec4 textDisabled = ImGuiIntRGBToFloatRGB(235, 219, 178);

    // backgrounds for the things that are not windows
    // buttons and tabs and such
    ImVec4 background1 = ImGuiIntRGBToFloatRGB(60, 56, 54);
    ImVec4 background2 = ImGuiIntRGBToFloatRGB(80, 73, 69);
    ImVec4 background3 = ImGuiIntRGBToFloatRGB(102, 92, 84);

    ImVec4 active = ImGuiIntRGBToFloatRGB(152, 151, 26); // green
    ImVec4 hovered1 = ImGuiIntRGBToFloatRGB(184, 187, 38);
    ImVec4 hovered2 = ImGuiIntRGBToFloatRGB(184, 187, 38);

    style->Colors[ImGuiCol_Tab] = active;
    style->Colors[ImGuiCol_TabActive] = windowBackground;
    style->Colors[ImGuiCol_TabHovered] = hovered2;
    style->Colors[ImGuiCol_TabDimmedSelected] = active;
    style->Colors[ImGuiCol_TabDimmedSelectedOverline] = active;
    style->Colors[ImGuiCol_TabSelectedOverline] = background1;

    style->Colors[ImGuiCol_FrameBg] = background1;
    style->Colors[ImGuiCol_FrameBgHovered] = hovered2;
    style->Colors[ImGuiCol_FrameBgActive] = active;

    style->Colors[ImGuiCol_Text] = textColor;
    style->Colors[ImGuiCol_TextDisabled] = textDisabled;
    style->Colors[ImGuiCol_WindowBg] = windowBackground;
    style->Colors[ImGuiCol_Border] = darkWindowBackground;
    style->Colors[ImGuiCol_BorderShadow] = darkWindowBackground;

    style->Colors[ImGuiCol_TitleBg] = background1;
    style->Colors[ImGuiCol_TitleBgCollapsed] = background3;
    style->Colors[ImGuiCol_TitleBgActive] = active;

    style->Colors[ImGuiCol_MenuBarBg] = darkWindowBackground;

    style->Colors[ImGuiCol_ScrollbarBg] = background1;
    style->Colors[ImGuiCol_ScrollbarGrab] = darkWindowBackground;
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = hovered2;
    style->Colors[ImGuiCol_ScrollbarGrabActive] = active;
    style->Colors[ImGuiCol_CheckMark] = active;

    style->Colors[ImGuiCol_SliderGrab] = background3;
    style->Colors[ImGuiCol_SliderGrabActive] = active;

    style->Colors[ImGuiCol_Button] = background1;
    style->Colors[ImGuiCol_ButtonHovered] = hovered1;
    style->Colors[ImGuiCol_ButtonActive] = active;

    style->Colors[ImGuiCol_Header] = background1;
    style->Colors[ImGuiCol_HeaderHovered] = hovered1;
    style->Colors[ImGuiCol_HeaderActive] = active;

    style->Colors[ImGuiCol_Separator] = darkWindowBackground;
    style->Colors[ImGuiCol_SeparatorHovered] = darkWindowBackground;

    style->Colors[ImGuiCol_SeparatorActive] = active;
    style->Colors[ImGuiCol_ResizeGrip] = background1;
    style->Colors[ImGuiCol_ResizeGripHovered] = hovered2;
    style->Colors[ImGuiCol_ResizeGripActive] = active;
    style->Colors[ImGuiCol_PlotLines] = active;
    style->Colors[ImGuiCol_PlotLinesHovered] = active;
    style->Colors[ImGuiCol_PlotHistogram] = active;
    style->Colors[ImGuiCol_PlotHistogramHovered] = active;
    style->Colors[ImGuiCol_TextSelectedBg] = textColor;
    style->Colors[ImGuiCol_PopupBg] = background2;

    style->Colors[ImGuiCol_DockingPreview] = active;
    style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0, 0, 0, 0);
}

}
