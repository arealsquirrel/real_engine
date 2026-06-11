
#include <cstdlib>
#include <cstring>
#include <real/graphics/render_pass_compute.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "real/core/game.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_shader.hpp"
#include "vulkan_descriptor_builder.hpp"
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
		std::vector<RenderPassResource> _resources)
	: RenderPassCompute(_instance, shader.get()->get_layout(), _resources) {

	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();

	DescriptorLayoutBuilder lb;
	VulkanResourceShader *vshader = (VulkanResourceShader*)shader.get();
	for(size_t i = 0; i < vshader->descriptor_types.size(); i++) {
		lb.add_binding(i, vshader->descriptor_types[i]);
	}

	descriptor_set_layout = lb.build(renderer->device, VK_SHADER_STAGE_COMPUTE_BIT);
	descriptor_set = renderer->descriptor_allocator.allocate(renderer->device, descriptor_set_layout);	
	DescriptorWriter writer;
	writer.write_image(0, ((VulkanResourceImage*)_resources[0].texture.get())->imageView, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
	writer.update_set(renderer->device, descriptor_set);

	VkPushConstantRange pushConstant{};
	pushConstant.offset = 0;
	pushConstant.size = 128;
	pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	
	VkPipelineLayoutCreateInfo computeLayout{};
	computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computeLayout.pNext = nullptr;
	computeLayout.pSetLayouts = &descriptor_set_layout;
	computeLayout.setLayoutCount = 1;
	computeLayout.pPushConstantRanges = &pushConstant;
	computeLayout.pushConstantRangeCount = 1;

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
	push_constant_buffer = (char*)malloc(128);
	memset(push_constant_buffer, 0, 128);
}

VulkanRenderPassCompute::~VulkanRenderPassCompute() {
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();
	free(push_constant_buffer);

    vkDeviceWaitIdle(renderer->device);
	
	vkDestroyDescriptorSetLayout(renderer->device, descriptor_set_layout, nullptr);
	vkDestroyPipelineLayout(renderer->device, layout, nullptr);
    vkDestroyPipeline(renderer->device, pipeline, nullptr);
}

void VulkanRenderPassCompute::begin_pass() {
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();
	FrameDataVulkan &frame = renderer->get_current_frame();

	for (auto &resource : resources) {
		VulkanResourceImage *image = (VulkanResourceImage*)resource.texture.get();
		image->transition_image(VK_IMAGE_LAYOUT_GENERAL);
	}

	vkCmdBindPipeline(
		frame.main_command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
}

void VulkanRenderPassCompute::bind_descriptors() {
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();
	FrameDataVulkan &frame = renderer->get_current_frame();

	vkCmdBindDescriptorSets(
		frame.main_command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, layout, 0, 1, &descriptor_set, 0, nullptr);
}

void VulkanRenderPassCompute::dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) {
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();
	FrameDataVulkan &frame = renderer->get_current_frame();

	vkCmdPushConstants(
			frame.main_command_buffer, layout,
			VK_SHADER_STAGE_COMPUTE_BIT, 0, 128, push_constant_buffer);

	vkCmdDispatch(frame.main_command_buffer, groupCountX, groupCountY, groupCountZ);
}

void VulkanRenderPassCompute::end_pass() {
	/* does literaly nothing lmao */
}

void VulkanRenderPassCompute::set_variable(
		ShaderField field, char *data, size_t size) {
	switch (field.type) {
	case(ShaderFieldType::PUSH_CONSTANT): {
		char *write_pointer = (char*)(push_constant_buffer+field.offset);
		memcpy(write_pointer, data, size);
		return; 
	}

	default:
		break;
	}
}

}
