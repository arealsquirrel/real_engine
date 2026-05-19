
#include "real/graphics/graphics.hpp"
#include "real/resource/resource_image.hpp"
#include "vulkan_backend.hpp"
#include <cassert>
#include <cmath>
#include <optional>
#include <real/graphics/renderer.hpp>
#include <vulkan/vulkan_core.h>
#include "vulkan_util.hpp"
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

namespace real {

void style_imgui();

Renderer::Renderer(Instance *_instance, Window *_window)
    : window(_window), instance(_instance) {
    render_data = new RendererDataVulkan;
    RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    
    VkCommandPoolCreateInfo commandPoolInfo = vkutil::command_pool_create_info(window_backend->graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkFenceCreateInfo fenceCreateInfo = vkutil::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkutil::semaphore_create_info();

	for (int i = 0; i < VULKAN_FRAME_OVERLAP; i++) {
        /* COMMAND POOLS AND BUFFERS */
		VK_CHECK(vkCreateCommandPool(
            window_backend->device,
            &commandPoolInfo,
            nullptr, &rdata->frame_data[i].command_pool));
		VkCommandBufferAllocateInfo cmdAllocInfo = vkutil::command_buffer_allocate_info(
            rdata->frame_data[i].command_pool, 1);
		VK_CHECK(vkAllocateCommandBuffers(
            window_backend->device, &cmdAllocInfo,
             &rdata->frame_data[i].main_command_buffer));
	

        /* SYNC OBJECTS */
        VK_CHECK(vkCreateFence(
            window_backend->device,
            &fenceCreateInfo, nullptr,
            &rdata->frame_data[i].render_fence));
	
		VK_CHECK(vkCreateSemaphore(
            window_backend->device,
            &semaphoreCreateInfo,
            nullptr, &rdata->frame_data[i].swapchain_semaphores));
    }

    rdata->render_image = new ResourceImage(instance, std::nullopt, 500, 500, ColorFormat::RGB_FLOAT);

    VK_CHECK(vkCreateCommandPool(window_backend->device, &commandPoolInfo, nullptr, &rdata->imm_command_pool));
	VkCommandBufferAllocateInfo cmdAllocInfo = vkutil::command_buffer_allocate_info(rdata->imm_command_pool, 1);
	VK_CHECK(vkAllocateCommandBuffers(window_backend->device, &cmdAllocInfo, &rdata->imm_command_buffer));
    VK_CHECK(vkCreateFence(window_backend->device, &fenceCreateInfo, nullptr, &rdata->imm_fence));

    /* ----- init imgui ----- */
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

	VK_CHECK(vkCreateDescriptorPool(window_backend->device, &pool_info, nullptr, &rdata->imgui_descriptor_pool));

	ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    style_imgui();

    ImGui_ImplGlfw_InitForVulkan(window->glfw_window(), true);

	// this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = backend->instance;
	init_info.PhysicalDevice = window_backend->chosenGPU;
	init_info.Device = window_backend->device;
	init_info.Queue = window_backend->graphics_queue;
	init_info.DescriptorPool = rdata->imgui_descriptor_pool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.UseDynamicRendering = true;
    // init_info.PipelineInfoMain

    ImGui_ImplVulkan_PipelineInfo pipe_info = {};
	pipe_info.PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
	pipe_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	pipe_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &window_backend->swapchain_image_format;
	pipe_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.PipelineInfoMain = pipe_info;

	ImGui_ImplVulkan_Init(&init_info);

	// ImGui_ImplVulkan_CreateFontsTexture();

	// add the destroy the imgui created structures
}

Renderer::~Renderer() {
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 

    vkDeviceWaitIdle(window_backend->device);

    ImGui_ImplVulkan_Shutdown();
	vkDestroyDescriptorPool(window_backend->device, rdata->imgui_descriptor_pool, nullptr);

    delete rdata->render_image;

    for (int i = 0; i < VULKAN_FRAME_OVERLAP; i++) {
        vkDestroyCommandPool(window_backend->device, rdata->frame_data[i].command_pool, nullptr);

	    vkDestroyFence(window_backend->device, rdata->frame_data[i].render_fence, nullptr);
	    vkDestroySemaphore(window_backend->device, rdata->frame_data[i].swapchain_semaphores, nullptr);
    
        rdata->frame_data[i].delete_queue.flush();
    }

	vkDestroyFence(window_backend->device, rdata->imm_fence, nullptr);
    vkDestroyCommandPool(window_backend->device, rdata->imm_command_pool, nullptr);

    rdata->delete_queue.flush();

    delete (RendererDataVulkan*)render_data;
}

FrameContext Renderer::start_frame() {
    RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    FrameDataVulkan *frame = &rdata->frame_data[frame_number % VULKAN_FRAME_OVERLAP];
    ImageVulkan *render_image_handle = (ImageVulkan*) rdata->render_image->get_handle();

    frame->draw_extent.width = render_image_handle->imageExtent.width;
    frame->draw_extent.height = render_image_handle->imageExtent.height;

    VK_CHECK(vkWaitForFences(
        window_backend->device, 1,
        &frame->render_fence, true, 1000000000));
	VK_CHECK(vkResetFences(
        window_backend->device, 1,
        &frame->render_fence));
	VK_CHECK(vkAcquireNextImageKHR(
        window_backend->device,
        window_backend->swapchain,
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

void Renderer::end_frame(FrameContext context) {
    RendererDataVulkan *rdata = (RendererDataVulkan*)render_data; 
    WindowBackendVulkan *window_backend = (WindowBackendVulkan*)window->backend_handle(); 
    GraphicsBackendVulkan *backend = (GraphicsBackendVulkan*)Graphics::get_backend();
    FrameDataVulkan *frame = (FrameDataVulkan*)context;
    VkCommandBuffer cmd = frame->main_command_buffer;
    ImageVulkan *render_image_handle = (ImageVulkan*) rdata->render_image->get_handle();
    ImGui::Render();


	//transition the draw image and the swapchain image into their correct transfer layouts
	vkutil::transition_image(cmd, render_image_handle->image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vkutil::transition_image(cmd, window_backend->swapchain_images[frame->swapchain_index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	vkutil::copy_image_to_image(cmd, render_image_handle->image, window_backend->swapchain_images[frame->swapchain_index], frame->draw_extent, window_backend->swapchain_extent);
	vkutil::transition_image(cmd, window_backend->swapchain_images[frame->swapchain_index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkRenderingAttachmentInfo colorAttachment = vkutil::attachment_info(window_backend->swapchain_views[frame->swapchain_index], nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkRenderingInfo renderInfo = vkutil::rendering_info(window_backend->swapchain_extent, &colorAttachment, nullptr);
	vkCmdBeginRendering(cmd, &renderInfo);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	vkCmdEndRendering(cmd);

    vkutil::transition_image(cmd, window_backend->swapchain_images[frame->swapchain_index], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    VK_CHECK(vkEndCommandBuffer(cmd));
    
    VkCommandBufferSubmitInfo cmdinfo = vkutil::command_buffer_submit_info(cmd);	
    VkSemaphoreSubmitInfo waitInfo = vkutil::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        frame->swapchain_semaphores);
    VkSemaphoreSubmitInfo signalInfo = vkutil::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                window_backend->render_semaphore[frame->swapchain_index]);	
    VkSubmitInfo2 submit = vkutil::submit_info(
        &cmdinfo,&signalInfo
        ,&waitInfo);	
    VK_CHECK(vkQueueSubmit2(
        window_backend->graphics_queue, 1,
        &submit, frame->render_fence));

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &window_backend->swapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pWaitSemaphores = &window_backend->render_semaphore[frame->swapchain_index];
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &frame->swapchain_index;

    VK_CHECK(vkQueuePresentKHR(window_backend->graphics_queue, &presentInfo));
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

    style->Colors[ImGuiCol_MenuBarBg] = background3;
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
