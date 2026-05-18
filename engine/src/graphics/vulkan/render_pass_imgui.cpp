
#include <real/graphics/render_pass_imgui.hpp>
#include "vulkan_backend.hpp"
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

namespace real {

/*
RenderPassImGui::RenderPassImGui(Instance *_instance) 
    : RenderPass(_instance) {

    RenderPassVulkan *pass = new RenderPassVulkan;
	RendererDataVulkan *render_data = (RendererDataVulkan*)instance->renderer->get_handle();
    WindowBackendVulkan *wind = (WindowBackendVulkan*)(instance->window->backend_handle());
    GraphicsBackendVulkan *vulkan_backend = (GraphicsBackendVulkan*)Graphics::get_backend();

    VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
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
	
	VK_CHECK(vkCreateDescriptorPool(wind->device, &pool_info, nullptr, &pass->descriptor_pool));

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(instance->window->glfw_window(), true);

	// this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = vulkan_backend->instance;
	init_info.PhysicalDevice = wind->chosenGPU;
	init_info.Device = wind->device;
	init_info.Queue = wind->graphics_queue;
	init_info.DescriptorPool = pass->descriptor_pool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.UseDynamicRendering = true;

	// dynamic rendering parameters for imgui to use
	// init_info.PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
	// init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	// init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &_swapchainImageFormat;
	// init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_Init(&init_info);

	// ImGui_ImplVulkan_CreateFontsTexture();

	// add the destroy the imgui created structures
}

RenderPassImGui::~RenderPassImGui() {
    RenderPassVulkan *pass = (RenderPassVulkan*)data;
    WindowBackendVulkan *wind = (WindowBackendVulkan*)(instance->window->backend_handle());
    ImGui_ImplVulkan_Shutdown();
	vkDestroyDescriptorPool(wind->device, pass->descriptor_pool, nullptr);
}

void RenderPassImGui::bind(FrameContext context) {
    FrameDataVulkan *frame = (FrameDataVulkan*)context;
	RenderPassVulkan *pass = (RenderPassVulkan*)data;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void RenderPassImGui::draw(FrameContext context) {
    FrameDataVulkan *frame = (FrameDataVulkan*)context;
    WindowBackendVulkan *wind = (WindowBackendVulkan*)(instance->window->backend_handle());
	RenderPassVulkan *pass = (RenderPassVulkan*)data;

    VkRenderingAttachmentInfo colorAttachment = vkutil::attachment_info(wind->swapchain_views[frame->swapchain_index], nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkRenderingInfo renderInfo = {}; //vkutil::rendering_info(wind->swapchain_extent, &colorAttachment, nullptr);
    

	vkCmdBeginRendering(frame->main_command_buffer, &renderInfo);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frame->main_command_buffer);

	vkCmdEndRendering(frame->main_command_buffer);
}
*/

}
