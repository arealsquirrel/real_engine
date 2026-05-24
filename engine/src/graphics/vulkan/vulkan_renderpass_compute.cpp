
#include <real/graphics/render_pass_compute.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "real/core/instance.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_shader.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_renderpass_compute.hpp"
#include "vulkan_renderer.hpp"
#include <real/real.hpp>

#include "vulkan_resource_image.hpp"
#include "vulkan_resource_shader.hpp"

namespace real {

VulkanRenderPassCompute::VulkanRenderPassCompute(
		Instance *_instance,
        ResourceHandle<ResourceShader> shader,
		std::vector<ResourceHandle<ResourceImage>> _inResources,
		std::vector<ResourceHandle<ResourceImage>> _outResources)
	: RenderPassCompute(_instance, _inResources, _outResources) {


	if(shader.get()->type != ShaderType::COMPUTE) {
		instance->log.warn("why are you passing a shader that isnt a compute shader to a compute pipeline. dumb ass");
	}

	// pass->descriptor_set_layout = vkutil::make_descriptor_set_array(
	// shader->fields, wind->device, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, nullptr, 0);

	VulkanRenderer *renderer = (VulkanRenderer*)_instance->renderer.get();
	descriptor_set = renderer->descriptor_allocator.allocate(renderer->device, descriptor_set_layout);	

	VkDescriptorImageInfo imgInfo{};
	imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imgInfo.imageView = ((VulkanResourceImage*)_inResources[0].get())->imageView;
	
	VkWriteDescriptorSet drawImageWrite = {};
	drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	drawImageWrite.pNext = nullptr;
	
	drawImageWrite.dstBinding = 0;
	drawImageWrite.dstSet = descriptor_set;
	drawImageWrite.descriptorCount = 1;
	drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	drawImageWrite.pImageInfo = &imgInfo;

	VkPipelineLayoutCreateInfo computeLayout{};
	computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computeLayout.pNext = nullptr;
	computeLayout.pSetLayouts = &descriptor_set_layout;
	computeLayout.setLayoutCount = 1;

	VK_CHECK(vkCreatePipelineLayout(renderer->device, &computeLayout, nullptr, &layout));

    VkPipelineShaderStageCreateInfo stageinfo{};
	stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageinfo.pNext = nullptr;
	stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	stageinfo.module = ((VulkanResourceShader*)shader.get())->module;
	stageinfo.pName = "main";

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.pNext = nullptr;
	computePipelineCreateInfo.layout = layout;
	computePipelineCreateInfo.stage = stageinfo;
	
	VK_CHECK(vkCreateComputePipelines(renderer->device, VK_NULL_HANDLE,1,&computePipelineCreateInfo, nullptr, &pipeline));
}

VulkanRenderPassCompute::~VulkanRenderPassCompute() {
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();

    vkDeviceWaitIdle(renderer->device);
	
	vkDestroyDescriptorSetLayout(renderer->device, descriptor_set_layout, nullptr);
	vkDestroyPipelineLayout(renderer->device, layout, nullptr);
    vkDestroyPipeline(renderer->device, pipeline, nullptr);
}

void VulkanRenderPassCompute::begin_pass(FrameContext context) {
	FrameDataVulkan *frame = (FrameDataVulkan*)context;

	vkCmdBindPipeline(
		frame->main_command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

	vkCmdBindDescriptorSets(
		frame->main_command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, layout, 0, 1, &descriptor_set, 0, nullptr);

}

void VulkanRenderPassCompute::end_pass(FrameContext context) {
	FrameDataVulkan *frame = (FrameDataVulkan*)context;
	vkCmdDispatch(
			frame->main_command_buffer, std::ceil(frame->draw_extent.width / 16.0), std::ceil(frame->draw_extent.height / 16.0), 1);
}

}
