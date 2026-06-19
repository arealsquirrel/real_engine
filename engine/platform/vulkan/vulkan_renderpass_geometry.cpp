
#include "vulkan_renderpass_geometry.hpp"
#include "real/core/core.hpp"
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include "real/graphics/render_pass.hpp"
#include "real/graphics/render_pass_geometry.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_shader.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_descriptor_builder.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_resource_image.hpp"
#include "vulkan_resource_mesh.hpp"
#include "vulkan_resource_shader.hpp"
#include "vulkan_util.hpp"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <real/graphics/framebuffer.hpp>

namespace real {

VulkanRenderPassGeometry::VulkanRenderPassGeometry(
		Instance *_instance, RenderPassGeometryInfo info,
		std::vector<ResourceHandle<ResourceShader>> shaders,
		std::vector<RenderPassResource> _resources)
		: RenderPassGeometry(_instance, shaders[0].get()->get_layout(), _resources) {

	RL_INSTRUMENT_FUNCTION
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();

	DescriptorLayoutBuilder builder;
	for (auto field : shaders[0].get()->get_layout().fields) {
		if(field.type == ShaderFieldType::UNIFORM) {
			switch (field.data_type) {
			case ShaderDataType::SAMPLED_IMAGE:
				builder.add_binding(field.location, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
				break;
			case ShaderDataType::UNIFORM_BUFFER:
				builder.add_binding(field.location, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
				break;
			default:
				RL_LOG_WARN("We dont do uniforms of that type {}", ShaderDataType_to_string(field.data_type));
			}
		}
	}

	descriptor_set_layout = builder.build(renderer->device, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT);

	VkPushConstantRange pushConstant{};
	pushConstant.offset = 0;
	pushConstant.size = 128;
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptor_set_layout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
	VK_CHECK(vkCreatePipelineLayout(renderer->device, &pipelineLayoutCreateInfo, nullptr, &layout));

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = create_input_assembly(info);
	VkPipelineRasterizationStateCreateInfo rasterizer = create_rasterizer(info);
	VkPipelineDepthStencilStateCreateInfo depthStencil = create_depth(info);
	VkPipelineMultisampleStateCreateInfo multisampling = create_multisample_control(info);

	VkPipelineRenderingCreateInfo renderInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	std::vector<VkFormat> colorFormats;
	colorFormats.push_back(VK_FORMAT_R16G16B16A16_SFLOAT);
	renderInfo.colorAttachmentCount = colorFormats.size();
	renderInfo.pColorAttachmentFormats = colorFormats.data();
    renderInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;    

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	for (auto &s : shaders) {
		for(int i = 1; i < 8; i++) {
			VkPipelineShaderStageCreateInfo stage = {};
			stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stage.pNext = nullptr;
			stage.module = dynamic_cast<VulkanResourceShader*>(s.get())->module;
			if(CHECK_FLAG(s.get()->get_type(), BIT(i)) && BIT(i) == ShaderTypeFlag_VERTEX) {
				stage.pName = "vertex_main";
				stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
				shaderStages.push_back(stage);
			} else if(CHECK_FLAG(s.get()->get_type(), BIT(i)) && BIT(i) == ShaderTypeFlag_FRAGMENT) {
				stage.pName = "fragment_main";
				stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				shaderStages.push_back(stage);
			} else if(CHECK_FLAG(s.get()->get_type(), BIT(i)) && BIT(i) == ShaderTypeFlag_GEOMETRY) {
				stage.pName = "geometry_main";
				stage.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				shaderStages.push_back(stage);
			}
		}
	}

    VkGraphicsPipelineCreateInfo pipelineInfo = { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    pipelineInfo.pNext = &renderInfo;
    pipelineInfo.stageCount = (uint32_t)shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = layout;

	VkDynamicState state[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamicInfo.pDynamicStates = &state[0];
    dynamicInfo.dynamicStateCount = 2;
    pipelineInfo.pDynamicState = &dynamicInfo;

	if (vkCreateGraphicsPipelines(renderer->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
        fmt::println("failed to create pipeline");
    }

	push_constant_buffer = (char*)malloc(128);
	memset(push_constant_buffer, 0, 128);
	addr_loc = shader_layout.get_field("_vertex_buffer");
}

VkPipelineDepthStencilStateCreateInfo VulkanRenderPassGeometry::create_depth(
		RenderPassGeometryInfo info) {
	
	VkPipelineDepthStencilStateCreateInfo depth = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	if(info.depth) {
		RL_LOG_TRACE("Depthing it");
		depth.depthTestEnable = VK_TRUE;
		depth.depthWriteEnable = VK_TRUE;
	} else {
		depth.depthTestEnable = VK_FALSE;
		depth.depthWriteEnable = VK_FALSE;
	}

    depth.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth.depthBoundsTestEnable = VK_FALSE;
    depth.stencilTestEnable = VK_FALSE;
    depth.front = {};
    depth.back = {};
    depth.minDepthBounds = 0.f;
    depth.maxDepthBounds = 1.f;
	return depth;
}

VkPipelineInputAssemblyStateCreateInfo VulkanRenderPassGeometry::create_input_assembly(
	RenderPassGeometryInfo info) {

	VkPipelineInputAssemblyStateCreateInfo inputAssembly =  { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    inputAssembly.primitiveRestartEnable = VK_FALSE;
	switch (info.topology) {
	case GeometryTopology::Triangle_list:
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;
	case GeometryTopology::Line_list:
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		break;
	case GeometryTopology::Point_list:
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		break;
	}
	
	return inputAssembly;
}

VkPipelineRasterizationStateCreateInfo VulkanRenderPassGeometry::create_rasterizer(
	RenderPassGeometryInfo info) {

	VkPipelineRasterizationStateCreateInfo rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	switch (info.polygon_mode) {
	case GeometryPolygonMode::Fill:
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		break;
	case GeometryPolygonMode::Line:
		rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
		break;
	case GeometryPolygonMode::Point:
		rasterizer.polygonMode = VK_POLYGON_MODE_POINT;
		break;
	}
    
    rasterizer.lineWidth = 1.f;

	switch (info.cull_mode) {
	case GeometryCullMode::BACK:
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		break;
	case GeometryCullMode::FRONT:
		rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
		break;
	case GeometryCullMode::NONE:
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		break;
	}

	rasterizer.cullMode = VK_CULL_MODE_NONE;

	switch (info.front_face) {
	case GeometryFrontFace::Clockwise:
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		break;
	case GeometryFrontFace::CounterClockwise:
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		break;
	}

	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

	return rasterizer;
}

VkPipelineMultisampleStateCreateInfo VulkanRenderPassGeometry::create_multisample_control(
	RenderPassGeometryInfo info) {

	VkPipelineMultisampleStateCreateInfo multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = vkutil::MSAA_to_vulkan_counts(info.msaa);
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
	return multisampling;
}

VulkanRenderPassGeometry::~VulkanRenderPassGeometry() {
	RL_INSTRUMENT_FUNCTION
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();
	free(push_constant_buffer);
    vkDeviceWaitIdle(renderer->device);
	vkDestroyDescriptorSetLayout(renderer->device, descriptor_set_layout, nullptr);
	vkDestroyPipelineLayout(renderer->device, layout, nullptr);
    vkDestroyPipeline(renderer->device, pipeline, nullptr);
}

void VulkanRenderPassGeometry::begin_pass(Framebuffer *framebuffer) {
	RL_INSTRUMENT_FUNCTION
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();
	FrameDataVulkan &frame = renderer->get_current_frame();
	VulkanResourceImage *vimg = (VulkanResourceImage*)framebuffer->get_msaa_color_image().get();
	VulkanResourceImage *dimg = (VulkanResourceImage*)framebuffer->get_depth_image().get();

	VkRenderingAttachmentInfo depthAttachment = vkutil::depth_attachment_info(dimg->imageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
	VkRenderingAttachmentInfo colorAttachment = vkutil::attachment_info(vimg->imageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkExtent2D draw_extent;
	draw_extent.width = framebuffer->get_width();
	draw_extent.height = framebuffer->get_height();
	VkRenderingInfo renderInfo = vkutil::rendering_info(draw_extent, &colorAttachment, &depthAttachment);
	
	vimg->transition_image(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	dimg->transition_image(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

	vkCmdBeginRendering(frame.main_command_buffer, &renderInfo);
	vkCmdBindPipeline(frame.main_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	//set dynamic viewport and scissor
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = framebuffer->get_width();
	viewport.height = framebuffer->get_height();
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	vkCmdSetViewport(frame.main_command_buffer, 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = framebuffer->get_width();
	scissor.extent.height = framebuffer->get_height();

	vkCmdSetScissor(frame.main_command_buffer, 0, 1, &scissor);
	writer.clear();
	descriptor_set = renderer->get_current_frame().frameDescriptors.allocate(renderer->device, descriptor_set_layout);
}

void VulkanRenderPassGeometry::draw_mesh(ResourceHandle<ResourceMesh> mesh) {
	RL_INSTRUMENT_FUNCTION
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();
	FrameDataVulkan &frame = renderer->get_current_frame();
	VulkanResourceMesh *da_mesh = (VulkanResourceMesh*)mesh.get();
	RenderPass::set_variable("_vertex_buffer", da_mesh->address);

	vkCmdPushConstants(frame.main_command_buffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, 128, push_constant_buffer);
	vkCmdBindIndexBuffer(frame.main_command_buffer, da_mesh->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(frame.main_command_buffer, da_mesh->indices_count, 1, 0, 0, 0);
	renderer->stats.indicies += da_mesh->indices_count;
	renderer->stats.verticies += da_mesh->verticie_count;
}

void VulkanRenderPassGeometry::bind_descriptors() {
	RL_INSTRUMENT_FUNCTION
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();
	writer.update_set(renderer->device, descriptor_set);
	vkCmdBindDescriptorSets(renderer->get_current_frame().main_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptor_set, 0, nullptr);
}

void VulkanRenderPassGeometry::end_pass() {
	RL_INSTRUMENT_FUNCTION
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();
	FrameDataVulkan &frame = renderer->get_current_frame();
	vkCmdEndRendering(frame.main_command_buffer);
}

void VulkanRenderPassGeometry::set_variable(
		ShaderField field, char *data, size_t size) {

	RL_INSTRUMENT_FUNCTION
	VulkanRenderer *renderer = (VulkanRenderer*)instance->renderer.get();

	switch (field.type) {
	case(ShaderFieldType::PUSH_CONSTANT): {
		memcpy(push_constant_buffer+field.offset, data, size);
		return; 
	}

	case (ShaderFieldType::UNIFORM): {
		if(field.data_type == ShaderDataType::SAMPLED_IMAGE) {
			VkImageView view = *(VkImageView*)data;
			writer.write_image(field.location, view, renderer->samplerLinear, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			return;
		} else if (field.data_type == ShaderDataType::UNIFORM_BUFFER) {
			VulkanUniformBuffer *buffer = *(VulkanUniformBuffer**)data;
			writer.write_buffer(field.location, buffer->buffer.buffer, buffer->size, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		}
	}

	default:
		break;
	}
}

}
 
