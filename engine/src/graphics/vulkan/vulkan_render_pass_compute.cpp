
#include <real/graphics/render_pass_compute.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "real/core/instance.hpp"
#include "real/graphics/render_pass.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_shader.hpp"
#include "vulkan_backend.hpp"
#include "vulkan_util.hpp"

namespace real {

/*

RenderPassCompute::RenderPassCompute(
    Instance *_instance,
    ResourceShader* shader)
    : RenderPass(_instance) {

    RenderPassVulkan *pass = new RenderPassVulkan;
	RendererDataVulkan *render_data = (RendererDataVulkan*)instance->renderer->get_handle();
    WindowBackendVulkan *wind = (WindowBackendVulkan*)(instance->window->backend_handle());

	if(shader->type != ShaderType::COMPUTE) {
		instance->log.warn("why are you passing a shader that isnt a compute shader to a compute pipeline. dumb ass");
	}

	pass->descriptor_set_layout = vkutil::make_descriptor_set_array(
	shader->fields, wind->device, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, nullptr, 0);

	pass->descriptor_set = wind->descriptor_allocator.allocate(wind->device, pass->descriptor_set_layout);	

    ImageVulkan *render_image_handle = (ImageVulkan*) render_data->render_image->get_handle();
	VkDescriptorImageInfo imgInfo{};
	imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imgInfo.imageView = render_image_handle->imageView; // need to do images
	
	VkWriteDescriptorSet drawImageWrite = {};
	drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	drawImageWrite.pNext = nullptr;
	
	drawImageWrite.dstBinding = 0;
	drawImageWrite.dstSet = pass->descriptor_set;
	drawImageWrite.descriptorCount = 1;
	drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	drawImageWrite.pImageInfo = &imgInfo;

	vkUpdateDescriptorSets(wind->device, 1, &drawImageWrite, 0, nullptr);


	VkPipelineLayoutCreateInfo computeLayout{};
	computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computeLayout.pNext = nullptr;
	computeLayout.pSetLayouts = &pass->descriptor_set_layout;
	computeLayout.setLayoutCount = 1;

	VK_CHECK(vkCreatePipelineLayout(wind->device, &computeLayout, nullptr, &pass->layout));

    VkPipelineShaderStageCreateInfo stageinfo{};
	stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageinfo.pNext = nullptr;
	stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	stageinfo.module = ((ShaderVulkan*)shader->get_handle())->module; // we will leave it here ig
	stageinfo.pName = "main";

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.pNext = nullptr;
	computePipelineCreateInfo.layout = pass->layout;
	computePipelineCreateInfo.stage = stageinfo;
	
	VK_CHECK(vkCreateComputePipelines(wind->device, VK_NULL_HANDLE,1,&computePipelineCreateInfo, nullptr, &pass->pipeline));

	
    data = pass;
}

RenderPassCompute::~RenderPassCompute() {
    RenderPassVulkan *pass = (RenderPassVulkan*)data;
    WindowBackendVulkan *wind = (WindowBackendVulkan*)instance->window->backend_handle();

	// vkDestroyDescriptorSetLayout(wind->device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks *pAllocator)
    vkDeviceWaitIdle(wind->device);
	
	vkDestroyDescriptorSetLayout(wind->device, pass->descriptor_set_layout, nullptr);
	vkDestroyPipelineLayout(wind->device, pass->layout, nullptr);
    vkDestroyPipeline(wind->device, pass->pipeline, nullptr);

    delete pass;
}

void RenderPassCompute::bind(FrameContext context) {
	FrameDataVulkan *frame = (FrameDataVulkan*)context;
	RenderPassVulkan *pass = (RenderPassVulkan*)data;
	vkCmdBindPipeline(
		frame->main_command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pass->pipeline);

	vkCmdBindDescriptorSets(
		frame->main_command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pass->layout, 0, 1, &pass->descriptor_set, 0, nullptr);

}

void RenderPassCompute::draw(FrameContext context) {
	FrameDataVulkan *frame = (FrameDataVulkan*)context;
	RenderPassVulkan *pass = (RenderPassVulkan*)data;

	// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
	vkCmdDispatch(
		frame->main_command_buffer, std::ceil(frame->draw_extent.width / 16.0), std::ceil(frame->draw_extent.height / 16.0), 1);

}
    
*/

}
